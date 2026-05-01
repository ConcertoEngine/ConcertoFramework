#include <cstring>
#include <cxxopts.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <Concerto/Core/Logger/Logger.hpp>

#include "Concerto/Core/Defines.hpp"
#include "Concerto/PackageGenerator/ChunkedStreamBuf.hpp"
#include "Concerto/PackageGenerator/ClangParser/ClangParser.hpp"
#include "Concerto/PackageGenerator/Plugin/PluginRegistry.hpp"

class PackageVisitor
{
public:
	PackageVisitor(const Package& package, const cct::PluginRegistry::SortedPlugin& plugin, std::ostream& outputStream,
				   const std::vector<std::string>& headers) :
		m_outputStream(outputStream),
		m_package(&package),
		m_functionTable(plugin.functionTable),
		m_privateData(plugin.privateData)
	{
		// Build header pointers
		std::vector<const char*> headerPtrs;
		headerPtrs.reserve(headers.size());
		for (const auto& h : headers)
			headerPtrs.push_back(h.c_str());

		ctx.package = &package;
		ctx.outputStream = &m_outputStream;
		ctx.indentLevel = &m_indentLevel;
		ctx.pluginPrivateData = m_privateData;
		ctx.headers = headerPtrs.data();
		ctx.headerCount = headerPtrs.size();

		InvokeIfSet(m_functionTable->BeforePackageGeneration, reinterpret_cast<const CrpPackage*>(&package), reinterpret_cast<CrpGenerationContext*>(&ctx));
		InvokeIfSet(m_functionTable->OnPackageGeneration, reinterpret_cast<const CrpPackage*>(&package), reinterpret_cast<CrpGenerationContext*>(&ctx));
		{
			InvokeOnPackageElements(package);
		}
		InvokeIfSet(m_functionTable->AfterPackageGeneration, reinterpret_cast<const CrpPackage*>(&package), reinterpret_cast<CrpGenerationContext*>(&ctx));
	}

private:
	std::ostream& m_outputStream;
	std::size_t m_indentLevel = 0;
	std::string m_namespacePath;
	cct::GenerationContext ctx;
	const Package* m_package;
	const CrpPluginFunctionTable* m_functionTable;
	void* m_privateData;

	template<typename Func, typename... Args>
	void InvokeIfSet(Func func, Args&&... args)
	{
		if (func)
			func(std::forward<Args>(args)...);
	}

	void InvokeOnPackageElements(const Package& package)
	{
		for (const auto& enumeration : package.enums)
			InvokeOnEnumGeneration(enumeration);

		for (const auto& klass : package.classes)
			InvokeOnClassGeneration(klass);

		for (const auto& ns : package.namespaces)
			InvokeOnNamespaceGeneration(ns);
	}

	void InvokeOnEnumGeneration(const Enum& enumeration)
	{
		ctx.currentEnum = &enumeration;
		auto* crpEnum = reinterpret_cast<const CrpEnum*>(&enumeration);
		auto* crpCtx = reinterpret_cast<CrpGenerationContext*>(&ctx);

		InvokeIfSet(m_functionTable->BeforeEnumGeneration, crpEnum, crpCtx);
		InvokeIfSet(m_functionTable->OnEnumGeneration, crpEnum, crpCtx);
		for (const auto& value : enumeration.elements)
		{
			ctx.currentEnumElement = &value;
			auto* crpElem = reinterpret_cast<const CrpEnumElement*>(&value);
			InvokeIfSet(m_functionTable->BeforeEnumElementGeneration, crpElem, crpCtx);
			InvokeIfSet(m_functionTable->OnEnumElementGeneration, crpElem, crpCtx);
			InvokeIfSet(m_functionTable->AfterEnumElementGeneration, crpElem, crpCtx);
			ctx.currentEnumElement = nullptr;
		}
		InvokeIfSet(m_functionTable->AfterEnumGeneration, crpEnum, crpCtx);
		ctx.currentEnum = nullptr;
	}

	void InvokeOnClassGeneration(const Class& klass)
	{
		ctx.currentClass = &klass;
		auto* crpCls = reinterpret_cast<const CrpClass*>(&klass);
		auto* crpCtx = reinterpret_cast<CrpGenerationContext*>(&ctx);

		// Method wrapper classes must be generated BEFORE the class that references them
		// Skip for template classes since methods reference unresolved template parameters
		if (!klass.isTemplateClass)
		{
			InvokeOnClassMethods(klass);
		}

		if (klass.isGenericClass)
		{
			InvokeIfSet(m_functionTable->BeforeGenericClassGeneration, crpCls, crpCtx);
			InvokeIfSet(m_functionTable->OnGenericClassGeneration, crpCls, crpCtx);
		}
		else if (klass.isTemplateClass)
		{
			// Generate specialization classes before the template class that references them
			for (const auto& spec : klass.templateSpecializations)
			{
				InvokeIfSet(m_functionTable->BeforeTemplateSpecializationGeneration, crpCls, spec.c_str(), crpCtx);
				InvokeIfSet(m_functionTable->OnTemplateSpecializationGeneration, crpCls, spec.c_str(), crpCtx);
				InvokeIfSet(m_functionTable->AfterTemplateSpecializationGeneration, crpCls, spec.c_str(), crpCtx);
			}
			InvokeIfSet(m_functionTable->BeforeTemplateClassGeneration, crpCls, crpCtx);
			InvokeIfSet(m_functionTable->OnTemplateClassGeneration, crpCls, crpCtx);
		}
		else
		{
			InvokeIfSet(m_functionTable->BeforeClassGeneration, crpCls, crpCtx);
			InvokeIfSet(m_functionTable->OnClassGeneration, crpCls, crpCtx);
		}

		if (!klass.isGenericClass && !klass.isTemplateClass)
		{
			InvokeOnClassMembers(klass);
		}

		if (klass.isGenericClass)
			InvokeIfSet(m_functionTable->AfterGenericClassGeneration, crpCls, crpCtx);
		else if (klass.isTemplateClass)
			InvokeIfSet(m_functionTable->AfterTemplateClassGeneration, crpCls, crpCtx);
		else
			InvokeIfSet(m_functionTable->AfterClassGeneration, crpCls, crpCtx);

		ctx.currentClass = nullptr;
	}

	void InvokeOnClassMembers(const Class& klass)
	{
		auto* crpCtx = reinterpret_cast<CrpGenerationContext*>(&ctx);
		for (const auto& member : klass.members)
		{
			ctx.currentMember = &member;
			auto* crpMember = reinterpret_cast<const CrpClassMember*>(&member);
			InvokeIfSet(m_functionTable->BeforeMemberGeneration, crpMember, crpCtx);
			InvokeIfSet(m_functionTable->OnMemberGeneration, crpMember, crpCtx);
			InvokeIfSet(m_functionTable->AfterMemberGeneration, crpMember, crpCtx);
			ctx.currentMember = nullptr;
		}
	}

	void InvokeOnClassMethods(const Class& klass)
	{
		auto* crpCtx = reinterpret_cast<CrpGenerationContext*>(&ctx);
		for (const auto& method : klass.methods)
		{
			ctx.currentMethod = &method;
			auto* crpMethod = reinterpret_cast<const CrpClassMethod*>(&method);
			InvokeIfSet(m_functionTable->BeforeMethodGeneration, crpMethod, crpCtx);
			InvokeIfSet(m_functionTable->OnMethodGeneration, crpMethod, crpCtx);
			InvokeIfSet(m_functionTable->AfterMethodGeneration, crpMethod, crpCtx);
			ctx.currentMethod = nullptr;
		}
	}

	void InvokeOnNamespaceGeneration(const Namespace& ns)
	{
		ctx.currentNamespace = &ns;
		std::string previousPath = m_namespacePath;
		if (!m_namespacePath.empty())
			m_namespacePath += "::";
		m_namespacePath += ns.name;
		ctx.namespacePath = m_namespacePath.c_str();

		auto* crpNs = reinterpret_cast<const CrpNamespace*>(&ns);
		auto* crpCtx = reinterpret_cast<CrpGenerationContext*>(&ctx);

		InvokeIfSet(m_functionTable->BeforeNamespaceGeneration, crpNs, crpCtx);
		InvokeIfSet(m_functionTable->OnNamespaceGeneration, crpNs, crpCtx);
		{
			for (const auto& nestedNs : ns.namespaces)
				InvokeOnNamespaceGeneration(nestedNs);

			for (const auto& enumeration : ns.enums)
				InvokeOnEnumGeneration(enumeration);

			for (const auto& klass : ns.classes)
				InvokeOnClassGeneration(klass);
		}
		InvokeIfSet(m_functionTable->AfterNamespaceGeneration, crpNs, crpCtx);
		m_namespacePath = previousPath;
		ctx.namespacePath = m_namespacePath.empty() ? nullptr : m_namespacePath.c_str();
		ctx.currentNamespace = nullptr;
	}
};

void WaitForDebugger()
{
	while (!cct::IsDebuggerAttached())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

int main(int argc, const char** argv)
{
	std::string outputFolder;
	std::vector<std::string> includes;
	std::vector<std::string> defines;
	std::vector<std::string> sources;
	std::vector<std::string> headers;
	std::string resourceDir;
	std::string sdk;
	std::vector<std::string> plugins;
	std::string pluginDir;
	bool noBuiltinGenerators = false;

	try
	{
		cxxopts::Options options("Concerto Reflection Generator",
								 "Concerto Reflection Generator");

		options.add_options()("outputDir", "Output directory", cxxopts::value<std::string>())("I,include", "Include directories (one or more)", cxxopts::value<std::vector<std::string>>())("D,define", "Preprocessor defines (one or more)", cxxopts::value<std::vector<std::string>>())("s,source", "Source files (one or more)", cxxopts::value<std::vector<std::string>>())("H,header", "Header file to include in the generated cpp", cxxopts::value<std::vector<std::string>>())("R,resource-dir", "Clang resource directory", cxxopts::value<std::string>())("S,sdk", "SDK path", cxxopts::value<std::string>())("P,plugin", "Plugin paths (one or more)", cxxopts::value<std::vector<std::string>>())("plugin-dir", "Directory containing plugins to load", cxxopts::value<std::string>())("v,version", "1.0.0")("h,help", "Show help");

		options.parse_positional({"outputDir"});

		auto result = options.parse(argc, argv);

		if (result.count("help"))
		{
			std::cout << options.help({""}) << "\n";
			return 0;
		}

		if (result.count("version"))
		{
			std::cout << "1.0.0\n";
			return 0;
		}

		if (!result.count("outputDir"))
		{
			std::cerr << "Missing required 'outputDir' argument.\n";
			std::cerr << options.help({""}) << "\n";
			return EXIT_FAILURE;
		}
		outputFolder = result["outputDir"].as<std::string>();

		if (result.count("include"))
			includes = result["include"].as<std::vector<std::string>>();
		if (result.count("define"))
			defines = result["define"].as<std::vector<std::string>>();
		if (result.count("source"))
			sources = result["source"].as<std::vector<std::string>>();
		if (result.count("header"))
			headers = result["header"].as<std::vector<std::string>>();
		if (result.count("resource-dir"))
			resourceDir = result["resource-dir"].as<std::string>();
		if (result.count("sdk"))
			sdk = result["sdk"].as<std::string>();
		if (result.count("plugin"))
		{
			plugins = result["plugin"].as<std::vector<std::string>>();
		}
		if (result.count("plugin-dir"))
		{
			pluginDir = result["plugin-dir"].as<std::string>();
		}

		cct::ClangParser parser;
		Package* package = parser.Parse(includes, defines, sources, resourceDir, sdk);
		if (!package)
		{
			return EXIT_FAILURE;
		}

		cct::Logger::Info("Creating plugin registry...");
		cct::PluginRegistry pluginRegistry;
		for (const auto& pluginPath : plugins)
		{
			cct::Logger::Info("Loading plugin: {}", pluginPath);
			if (!pluginRegistry.LoadPlugin(pluginPath))
			{
				cct::Logger::Error("Failed to load plugin: {}", pluginPath);
			}
		}
		if (!pluginDir.empty())
		{
			pluginRegistry.LoadPluginsFromDirectory(pluginDir);
		}

		if (pluginRegistry.GetPlugins().empty())
		{
			cct::Logger::Error("No plugins loaded. Provide at least one plugin with -P or --plugin-dir.");
			return EXIT_FAILURE;
		}

		cct::Logger::Info("Transforming package...");
		pluginRegistry.TransformPackage(*package);

		cct::Logger::Info("Generating files via plugins...");

		std::filesystem::path outputPath(outputFolder);
		std::filesystem::create_directories(outputPath);

		for (const auto& plugin : pluginRegistry.GetPlugins())
		{
			CrpPluginInfo info{};
			plugin.functionTable->GetInfo(&info);

			if (info.outputFileExtension == nullptr || std::strlen(info.outputFileExtension) == 0)
			{
				cct::Logger::Error("Plugin '{}' returned invalid output file extension.", info.name != nullptr ? info.name : "Unknown");
				continue;
			}

			std::string extension = info.outputFileExtension;
			std::filesystem::path outPath = outputPath / (package->name + "Package" + extension);

			std::ofstream outFile(outPath);
			if (!outFile)
			{
				cct::Logger::Error("Failed to open file for writing: {}", outPath.string());
				continue;
			}

			{
				cct::AsyncChunkedStreamBuf asyncBuf(outFile);
				std::ostream asyncStream(&asyncBuf);

				PackageVisitor visitor(*package, plugin, asyncStream, headers);
			}

			if (outFile.tellp() <= 0)
			{
				outFile.close();
				std::filesystem::remove(outPath);
			}
			else
			{
				cct::Logger::Info("Generated file: {}", outPath.string());
			}
		}

		cct::Logger::Info("Done!");
	}
	catch (const std::exception& e)
	{
		cct::Logger::Error("Error: {}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
