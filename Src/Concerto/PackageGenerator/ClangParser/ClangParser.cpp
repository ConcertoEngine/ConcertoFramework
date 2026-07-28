//
// Clang LibTooling based parser for Concerto Package Generator
//

#include "Concerto/PackageGenerator/ClangParser/ClangParser.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <system_error>

#include <Concerto/Core/Logger/Logger.hpp>
#include <Concerto/Profiler/Profiler.hpp>

#include <clang/AST/ASTContext.h>
#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/Type.h>
#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/PreprocessorOutputOptions.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Frontend/Utils.h>
#include <clang/Lex/Lexer.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/ADT/SmallString.h>
#include <toml11/parser.hpp>

using namespace clang;
using namespace clang::tooling;

namespace
{

	class MacroExpandAction : public PreprocessorFrontendAction
	{
	public:
		explicit MacroExpandAction(std::string& output) :
			m_output(output)
		{
		}

		void ExecuteAction() override
		{
			CompilerInstance& CI = getCompilerInstance();
			llvm::raw_string_ostream os(m_output);
			PreprocessorOutputOptions outOpts;
			DoPrintPreprocessedInput(CI.getPreprocessor(), &os, outOpts);
		}

	private:
		std::string& m_output;
	};

	TomlAttributes GetAttributesOr(TomlAttributes attributes, TomlAttributes defaultValue)
	{
		if (attributes.is_table() == false)
			return defaultValue;
		return attributes.as_table();
	}

	std::string QualTypeToString(const QualType& qt, const ASTContext& ctx)
	{
		CCT_AUTO_PROFILER_SCOPE();
		PrintingPolicy policy(ctx.getLangOpts());
		policy.SuppressTagKeyword = true;
		policy.Bool = true;
		policy.SuppressUnwrittenScope = true;
		policy.AnonymousTagLocations = false;
		return qt.getAsString(policy);
	}

	std::vector<std::string> GetNamespaceChain(const DeclContext* DC)
	{
		CCT_AUTO_PROFILER_SCOPE();
		std::vector<std::string> chain;
		while (DC)
		{
			if (const auto* ND = llvm::dyn_cast<NamespaceDecl>(DC))
			{
				if (!ND->isAnonymousNamespace())
					chain.emplace_back(ND->getNameAsString());
			}
			DC = DC->getParent();
		}
		std::ranges::reverse(chain);
		return chain;
	}

	Namespace* EnsureNamespace(Package& pkg, const std::vector<std::string>& chain)
	{
		CCT_AUTO_PROFILER_SCOPE();
		Namespace* current = nullptr;
		auto* level = &pkg.namespaces;
		for (const auto& name : chain)
		{
			auto it = std::ranges::find_if(*level, [&](const Namespace& ns)
										   { return ns.name == name; });
			if (it == level->end())
			{
				Namespace ns;
				ns.name = name;
				level->push_back(std::move(ns));
				current = &level->back();
			}
			else
			{
				current = &*it;
			}
			level = &current->namespaces;
		}
		return current; // nullptr if chain empty
	}

	bool ExtractCctAttribute(const Attr* A, ASTContext& astContext, const SourceManager& SM, const LangOptions& LO,
							 std::string& outScope, TomlAttributes& outAttrs)
	{
		CCT_AUTO_PROFILER_SCOPE();
		const IdentifierInfo* identifierInfo = A->getAttrName();
		auto attributeName = identifierInfo ? identifierInfo->getName() : StringRef();
		const auto* AnnotateAttribute = dyn_cast<AnnotateAttr>(A);
		if (AnnotateAttribute == nullptr)
			return false;

		using namespace std::string_view_literals;

		constexpr std::array knownAnnotations = {
			"cct::Package"sv,
			"cct::Class"sv,
			"cct::GenericClass"sv,
			"cct::GenericType"sv,
			"cct::Member"sv,
			"cct::NativeMember"sv,
			"cct::Method"sv,
			"cct::Enum"sv,
			"cct::EnumValue"sv};

		std::string_view annotation(AnnotateAttribute->getAnnotation().begin(), AnnotateAttribute->getAnnotation().end());
		bool known = false;

		for (const auto& annotationName : knownAnnotations)
		{
			if (annotationName == annotation)
			{
				known = true;
				break;
			}
		}

		if (!known)
			return false;

		outScope = annotation;

		std::string tomlStr;
		for (auto& args : AnnotateAttribute->args())
		{
			auto str = args->tryEvaluateString(astContext);
			if (str)
				tomlStr += *str + '\n';
		}
		tomlStr += '\n';
		auto result = toml::try_parse_str(tomlStr);
		if (result.is_err())
		{
			for (auto& err : result.as_err())
				cct::Logger::Error("{}\n\t{}", err.title(), err.suffix());
			return {};
		}

		outAttrs = result.as_ok();
		return true;
	}

	void InsertClassIntoPackage(Package& pkg, const std::vector<std::string>& nsChain, const Class& klass)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (nsChain.empty())
		{
			pkg.classes.push_back(klass);
			return;
		}
		Namespace* leaf = EnsureNamespace(pkg, nsChain);
		if (leaf)
			leaf->classes.push_back(klass);
	}

	void InsertEnumIntoPackage(Package& pkg, const std::vector<std::string>& nsChain, const Enum& enumeration)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (nsChain.empty())
		{
			pkg.enums.push_back(enumeration);
			return;
		}
		Namespace* leaf = EnsureNamespace(pkg, nsChain);
		if (leaf)
		{
			auto it = std::ranges::find_if(leaf->enums, [&](const Enum& e)
										   { return e.name == enumeration.name; });

			if (it == leaf->enums.end())
				leaf->enums.push_back(enumeration);
		}
	}
} // namespace

namespace cct
{
	Package* ClangParser::Parse(const std::vector<std::string>& includeDirs,
								const std::vector<std::string>& defines,
								const std::vector<std::string>& sources,
								const std::string& resourceDir,
								const std::string& sdk)
	{
		m_sourcePaths.clear();
		for (const auto& src : sources)
		{
			std::error_code ec;
			auto canonical = std::filesystem::weakly_canonical(src, ec);
			m_sourcePaths.insert(ec ? src : canonical.string());
		}

		std::string code;

		code += "#include <Concerto/Core/Defines.hpp>\n";
		code += "#undef CCT_EXPORT\n";
		code += "#define CCT_EXPORT\n";
		code += "#undef CCT_IMPORT\n";
		code += "#define CCT_IMPORT\n";

		for (auto& src : sources)
			code += std::format("#include \"{}\"\n", src);

		std::vector<std::string> args;
		args.reserve(defines.size() + includeDirs.size() + 3);

		args.emplace_back("-std=c++20");
		args.emplace_back("-xc++");
		args.emplace_back("-resource-dir=" + resourceDir);
		args.emplace_back("-fsyntax-only");
#ifdef CCT_PLATFORM_MACOS
		args.emplace_back("-isysroot" + sdk);
#endif

		for (auto& define : defines)
			args.emplace_back("-D" + define);

		for (auto& include : includeDirs)
			args.emplace_back("-I" + include);

		DiagnosticOptions diagOpts;
		diagOpts.ShowColors = true;
		TextDiagnosticPrinter diagPrinter(llvm::outs(), diagOpts);

		std::unique_ptr<ASTUnit> AST;
		{
			CCT_AUTO_PROFILER_SCOPE();
			AST = buildASTFromCodeWithArgs(code, args, "input.cc", "clang-tool",
										   std::make_shared<PCHContainerOperations>(),
										   getClangStripDependencyFileAdjuster(),
										   FileContentMappings(),
										   &diagPrinter);
		}

		if (!AST)
		{
			Logger::Error("LibTooling: failed to parse");
			return nullptr;
		}

		m_astContext = &AST->getASTContext();
		m_sourceManager = &m_astContext->getSourceManager();
		m_langOptions = &m_astContext->getLangOpts();

		const TranslationUnitDecl* TU = AST->getASTContext().getTranslationUnitDecl();
		for (const auto* D : TU->decls())
			ProcessDeclaration(D);

		RemoveEmptyNamespaces(m_package.namespaces);

		return &m_package;
	}

	bool ClangParser::IsInSourceFile(const clang::Decl* decl) const
	{
		if (m_sourcePaths.empty())
			return true;
		auto loc = decl->getLocation();
		if (!loc.isValid())
			return false;
		auto presumedLoc = m_sourceManager->getPresumedLoc(loc);
		if (presumedLoc.isInvalid())
			return false;
		std::error_code ec;
		auto canonical = std::filesystem::weakly_canonical(presumedLoc.getFilename(), ec);
		const std::string filePath = ec ? presumedLoc.getFilename() : canonical.string();
		return m_sourcePaths.count(filePath) > 0;
	}

	void ClangParser::ProcessRecord(const CXXRecordDecl* recordDeclaration)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!recordDeclaration || !recordDeclaration->isThisDeclarationADefinition())
			return;
		if (!IsInSourceFile(recordDeclaration))
			return;

		std::optional<std::pair<std::string, TomlAttributes>> classAttr;
		std::optional<std::pair<std::string, TomlAttributes>> packageAttr;
		std::optional<std::pair<std::string, TomlAttributes>> genericClassAttr;
		for (const auto* A : recordDeclaration->attrs())
		{
			std::string scope;
			TomlAttributes attrs;
			if (!ExtractCctAttribute(A, *m_astContext, *m_sourceManager, *m_langOptions, scope, attrs))
				continue;
			if (scope == "cct::Class")
				classAttr = std::make_pair(scope, attrs);
			else if (scope == "cct::Package")
				packageAttr = std::make_pair(scope, attrs);
			else if (scope == "cct::GenericClass")
				genericClassAttr = std::make_pair(scope, attrs);
		}
		auto txt = recordDeclaration->getNameAsString();
		if (!classAttr && !packageAttr && !genericClassAttr)
			return; // Not a reflection-relevant class

		if (packageAttr)
		{
			if (m_package.name.empty())
			{
				m_package.name = recordDeclaration->getNameAsString();
				auto& attributesTable = packageAttr->second.as_table();
				auto versionIt = attributesTable.find("version");
				m_package.version = versionIt != attributesTable.end() && versionIt->second.is_string() ? versionIt->second.as_string() : std::string("Undefined");
				auto descriptionIt = attributesTable.find("description");
				m_package.description = descriptionIt != attributesTable.end() && descriptionIt->second.is_string() ? descriptionIt->second.as_string() : std::string("Undefined");
			}
			else if (m_package.name != recordDeclaration->getNameAsString())
			{
				Logger::Error("Package has already been defined");
			}
			return;
		}

		// Determine which type of class this is
		if (!classAttr && !genericClassAttr)
			return; // Only keep classes with cct::Class or cct::GenericClass

		Class cls;
		cls.name = recordDeclaration->getNameAsString();

		// Handle generic class
		if (genericClassAttr)
		{
			cls.scope = "cct::GenericClass";
			cls.isGenericClass = true;
			cls.tomlAttributes = GetAttributesOr(genericClassAttr->second, {});

			// Detect explicit type parameters via CCT_GENERIC_TYPE() annotations
			for (const auto* F : recordDeclaration->fields())
			{
				auto fieldName = F->getNameAsString();
				auto fieldType = QualTypeToString(F->getType(), *m_astContext);

				// Check if field has CCT_GENERIC_TYPE annotation
				for (const auto* attr : F->getAttrs())
				{
					if (const auto* annotationAttr = dyn_cast<clang::AnnotateAttr>(attr))
					{
						auto annotation = annotationAttr->getAnnotation().str();
						if (annotation == "cct::GenericType")
						{
							// Verify field type is "const Class*"
							if (fieldType == "const cct::refl::Class *" ||
								fieldType == "const Class *" ||
								fieldType == "const cct::refl::Class*" ||
								fieldType == "const Class*")
							{
								cls.genericTypeParameterFields.push_back(fieldName);
							}
							break;
						}
					}
				}
			}
		}
		else
		{
			cls.scope = "cct::Class";
			cls.tomlAttributes = GetAttributesOr(classAttr->second, {});
		}

		// Base class (first base)
		if (recordDeclaration->getNumBases() > 0)
		{
			const auto& base = *recordDeclaration->bases_begin();
			cls.base = QualTypeToString(base.getType(), *m_astContext);
		}

		// Fields with [[cct::Member(...)]]
		for (const auto* F : recordDeclaration->fields())
		{
			bool hasAttr = false;
			TomlAttributes memberAttrs;
			std::string scope;
			for (const auto* A : F->attrs())
			{
				TomlAttributes attrs;
				if (!ExtractCctAttribute(A, *m_astContext, *m_sourceManager, *m_langOptions, scope, attrs))
					continue;
				if (scope == "cct::Member" || scope == "cct::NativeMember")
				{
					hasAttr = true;
					memberAttrs = GetAttributesOr(attrs, {});
					break;
				}
			}
			if (!hasAttr)
				continue;

			Class::Member m;
			m.name = F->getNameAsString();
			m.type = QualTypeToString(F->getType(), *m_astContext);
			m.isNative = scope == "cct::NativeMember";
			m.tomlAttributes = memberAttrs;
			cls.members.push_back(std::move(m));
		}

		// Methods with [[cct::Method(...)]]
		for (const auto* M : recordDeclaration->methods())
		{
			bool hasAttr = false;
			TomlAttributes methodAttrs;
			for (const auto* A : M->attrs())
			{
				std::string scope;
				TomlAttributes attrs;
				if (!ExtractCctAttribute(A, *m_astContext, *m_sourceManager, *m_langOptions, scope, attrs))
					continue;
				if (scope == "cct::Method")
				{
					hasAttr = true;
					methodAttrs = GetAttributesOr(attrs, {});
					break;
				}
			}
			if (!hasAttr)
				continue;

			Class::Method mm;
			mm.name = M->getNameAsString();
			mm.returnValue = QualTypeToString(M->getReturnType(), *m_astContext);
			mm.tomlAttributes = methodAttrs;

			unsigned pi = 0;
			for (const auto* P : M->parameters())
			{
				Class::Method::Params param;
				param.type = QualTypeToString(P->getType(), *m_astContext);
				if (P->getIdentifier())
					param.name = P->getNameAsString();
				else
				{
					param.name = std::string("arg") + std::to_string(pi);
				}
				mm.params.push_back(std::move(param));
				++pi;
			}
			cls.methods.push_back(std::move(mm));
		}

		std::vector<std::string> nsChain = GetNamespaceChain(recordDeclaration->getDeclContext());
		InsertClassIntoPackage(m_package, nsChain, cls);
	}

	void ClangParser::ProcessEnum(const EnumDecl* enumDeclaration)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!enumDeclaration || !enumDeclaration->isThisDeclarationADefinition())
			return;
		if (!IsInSourceFile(enumDeclaration))
			return;
		bool hasAttr = false;
		TomlAttributes enumAttrs;
		for (const auto* A : enumDeclaration->attrs())
		{
			std::string scope;
			TomlAttributes attrs;
			if (!ExtractCctAttribute(A, *m_astContext, *m_sourceManager, *m_langOptions, scope, attrs))
				continue;
			if (scope == "cct::Enum")
			{
				hasAttr = true;
				enumAttrs = GetAttributesOr(attrs, {});
				break;
			}
		}
		if (!hasAttr)
			return;

		Enum enm;
		enm.name = enumDeclaration->getNameAsString();
		if (const TypeSourceInfo* TSI = enumDeclaration->getIntegerTypeSourceInfo())
			enm.base = QualTypeToString(TSI->getType(), *m_astContext);

		for (const auto* E : enumDeclaration->enumerators())
		{
			Enum::Element elem;
			elem.name = E->getNameAsString();
			// getInitVal() is always valid (computed value, explicit or implicit).
			llvm::APSInt val = E->getInitVal();
			llvm::SmallString<32> s;
			val.toString(s, /*Radix=*/10);
			elem.value.assign(s.begin(), s.end());
			enm.elements.push_back(std::move(elem));
		}

		std::vector<std::string> nsChain = GetNamespaceChain(enumDeclaration->getDeclContext());
		InsertEnumIntoPackage(m_package, nsChain, enm);
	}

	void ClangParser::ProcessNamespace(const NamespaceDecl* namespaceDeclaration)
	{
		CCT_AUTO_PROFILER_SCOPE();
		for (const auto* D : namespaceDeclaration->decls())
			ProcessDeclaration(D);
	}

	void ClangParser::ProcessDeclaration(const Decl* declaration)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (const auto* CTD = llvm::dyn_cast<ClassTemplateDecl>(declaration))
		{
			ProcessTemplateRecord(CTD);
		}
		else if (const auto* CTSD = llvm::dyn_cast<ClassTemplateSpecializationDecl>(declaration))
		{
			ProcessTemplateSpecialization(CTSD);
		}
		else if (const auto* RD = llvm::dyn_cast<CXXRecordDecl>(declaration))
		{
			ProcessRecord(RD);
		}
		else if (const auto* ED = llvm::dyn_cast<EnumDecl>(declaration))
		{
			ProcessEnum(ED);
		}
		else if (const auto* ND = llvm::dyn_cast<NamespaceDecl>(declaration))
		{
			auto nsName = ND->getNameAsString();
			// Visit nested declarations inside namespaces
			for (const auto* SD : ND->decls())
			{
				if (const auto* CTD2 = llvm::dyn_cast<ClassTemplateDecl>(SD))
					ProcessTemplateRecord(CTD2);
				else if (const auto* CTSD2 = llvm::dyn_cast<ClassTemplateSpecializationDecl>(SD))
					ProcessTemplateSpecialization(CTSD2);
				else if (const auto* RD2 = llvm::dyn_cast<CXXRecordDecl>(SD))
					ProcessRecord(RD2);
				else if (const auto* ED2 = llvm::dyn_cast<EnumDecl>(SD))
					ProcessEnum(ED2);
				else if (const auto* ND2 = llvm::dyn_cast<NamespaceDecl>(SD))
					ProcessNamespace(ND2);
			}
		}
	}

	void ClangParser::ProcessTemplateRecord(const ClassTemplateDecl* templateDeclaration)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!templateDeclaration)
			return;
		if (!IsInSourceFile(templateDeclaration))
			return;

		const auto* recordDecl = templateDeclaration->getTemplatedDecl();
		if (!recordDecl || !recordDecl->isThisDeclarationADefinition())
			return;

		std::optional<std::pair<std::string, TomlAttributes>> classAttr;
		for (const auto* A : recordDecl->attrs())
		{
			std::string scope;
			TomlAttributes attrs;
			if (!ExtractCctAttribute(A, *m_astContext, *m_sourceManager, *m_langOptions, scope, attrs))
				continue;
			if (scope == "cct::Class")
			{
				classAttr = std::make_pair(scope, attrs);
				break;
			}
		}

		if (!classAttr)
			return;

		Class cls;
		cls.name = recordDecl->getNameAsString();
		cls.scope = "cct::Class";
		cls.tomlAttributes = GetAttributesOr(classAttr->second, {});
		cls.isTemplateClass = true;

		const auto* paramList = templateDeclaration->getTemplateParameters();
		if (paramList)
		{
			for (unsigned i = 0; i < paramList->size(); ++i)
			{
				const auto* templateParam = paramList->getParam(i);
				::TemplateParameter tparam;
				if (const auto* typeParam = llvm::dyn_cast<TemplateTypeParmDecl>(templateParam))
				{
					tparam.name = typeParam->getNameAsString();
					if (typeParam->hasDefaultArgument())
					{
						// TODO: Extract default argument
					}
				}
				else if (const auto* nonTypeParam = llvm::dyn_cast<NonTypeTemplateParmDecl>(templateParam))
				{
					tparam.name = nonTypeParam->getNameAsString();
				}

				if (!tparam.name.empty())
					cls.templateParameters.push_back(std::move(tparam));
			}
		}

		if (recordDecl->getNumBases() > 0)
		{
			const auto& base = *recordDecl->bases_begin();
			cls.base = QualTypeToString(base.getType(), *m_astContext);
		}

		for (const auto* F : recordDecl->fields())
		{
			bool hasAttr = false;
			TomlAttributes memberAttrs;
			std::string scope;
			for (const auto* A : F->attrs())
			{
				TomlAttributes attrs;
				if (!ExtractCctAttribute(A, *m_astContext, *m_sourceManager, *m_langOptions, scope, attrs))
					continue;
				if (scope == "cct::Member" || scope == "cct::NativeMember")
				{
					hasAttr = true;
					memberAttrs = GetAttributesOr(attrs, {});
					break;
				}
			}
			if (!hasAttr)
				continue;

			Class::Member m;
			m.name = F->getNameAsString();
			m.type = QualTypeToString(F->getType(), *m_astContext);
			m.isNative = scope == "cct::NativeMember";
			m.tomlAttributes = memberAttrs;
			cls.members.push_back(std::move(m));
		}

		for (const auto* M : recordDecl->methods())
		{
			bool hasAttr = false;
			TomlAttributes methodAttrs;
			for (const auto* A : M->attrs())
			{
				std::string scope;
				TomlAttributes attrs;
				if (!ExtractCctAttribute(A, *m_astContext, *m_sourceManager, *m_langOptions, scope, attrs))
					continue;
				if (scope == "cct::Method")
				{
					hasAttr = true;
					methodAttrs = GetAttributesOr(attrs, {});
					break;
				}
			}
			if (!hasAttr)
				continue;

			Class::Method mm;
			mm.name = M->getNameAsString();
			mm.returnValue = QualTypeToString(M->getReturnType(), *m_astContext);
			mm.tomlAttributes = methodAttrs;

			unsigned pi = 0;
			for (const auto* P : M->parameters())
			{
				Class::Method::Params param;
				param.type = QualTypeToString(P->getType(), *m_astContext);
				if (P->getIdentifier())
					param.name = P->getNameAsString();
				else
				{
					param.name = std::string("arg") + std::to_string(pi);
				}
				mm.params.push_back(std::move(param));
				++pi;
			}
			cls.methods.push_back(std::move(mm));
		}

		std::vector<std::string> nsChain = GetNamespaceChain(recordDecl->getDeclContext());
		InsertClassIntoPackage(m_package, nsChain, cls);
	}

	void ClangParser::ProcessTemplateSpecialization(const ClassTemplateSpecializationDecl* specializationDeclaration)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!specializationDeclaration || !specializationDeclaration->isThisDeclarationADefinition())
			return;

		const auto& templateArgs = specializationDeclaration->getTemplateArgs();
		std::vector<std::string> typeArgs;

		for (unsigned i = 0; i < templateArgs.size(); ++i)
		{
			const auto& arg = templateArgs[i];
			if (arg.getKind() == TemplateArgument::Type)
			{
				typeArgs.push_back(QualTypeToString(arg.getAsType(), *m_astContext));
			}
			else if (arg.getKind() == TemplateArgument::Integral)
			{
				llvm::SmallString<32> s;
				arg.getAsIntegral().toString(s, /*Radix=*/10);
				typeArgs.emplace_back(s.begin(), s.end());
			}
		}

		const auto* templateDecl = specializationDeclaration->getSpecializedTemplate();
		if (!templateDecl)
			return;

		const auto templateClassName = templateDecl->getNameAsString();

		std::vector<std::string> nsChain = GetNamespaceChain(specializationDeclaration->getDeclContext());
		Namespace* leaf = nsChain.empty() ? nullptr : EnsureNamespace(m_package, nsChain);
		auto& classList = nsChain.empty() ? m_package.classes : leaf->classes;

		auto it = std::ranges::find_if(classList, [&](const Class& c)
									   { return c.name == templateClassName && c.isTemplateClass; });

		if (it == classList.end())
			return;

		it->templateSpecializations.push_back(typeArgs.empty() ? "" : [&typeArgs]()
												  {
				std::string result;
				for (std::size_t i = 0; i < typeArgs.size(); ++i)
				{
					result += typeArgs[i];
					if (i < typeArgs.size() - 1)
						result += ",";
				}
				return result; }());
	}

	void ClangParser::RemoveEmptyNamespaces(std::vector<Namespace>& namespaces)
	{
		auto it = namespaces.begin();
		while (it != namespaces.end())
		{
			RemoveEmptyNamespaces(it->namespaces);

			bool isEmpty = it->classes.empty() && it->enums.empty() && it->namespaces.empty();

			if (isEmpty)
				it = namespaces.erase(it);
			else
				++it;
		}
	}
} // namespace cct
