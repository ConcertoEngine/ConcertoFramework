//
// Created by arthur
//

#include "Concerto/PackageGenerator/Plugin/PluginApi.h"

#include <cstdarg>
#include <random>
#include <ranges>

#include <Concerto/Profiler/Profiler.hpp>

#include "Concerto/Core/Assert.hpp"
#include "Concerto/PackageGenerator/Defines.hpp"
#include "Concerto/PackageGenerator/Plugin/ReflectionGeneratorPlugin.hpp"

extern "C"
{
	const char* crpPackageGetName(const CrpPackage* package)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!package)
			return nullptr;
		return reinterpret_cast<const Package*>(package)->name.c_str();
	}

	const char* crpPackageGetVersion(const CrpPackage* package)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!package)
			return nullptr;
		return reinterpret_cast<const Package*>(package)->version.c_str();
	}

	const char* crpPackageGetDescription(const CrpPackage* package)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!package)
			return nullptr;
		return reinterpret_cast<const Package*>(package)->description.c_str();
	}

	size_t crpPackageGetClassCount(const CrpPackage* package)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!package)
			return 0;
		return reinterpret_cast<const Package*>(package)->classes.size();
	}

	const CrpClass* crpPackageGetClass(const CrpPackage* package, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!package)
			return nullptr;
		const auto* pkg = reinterpret_cast<const Package*>(package);
		if (index >= pkg->classes.size())
			return nullptr;
		return reinterpret_cast<const CrpClass*>(&pkg->classes[index]);
	}

	size_t crpPackageGetNamespaceCount(const CrpPackage* package)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!package)
			return 0;
		return reinterpret_cast<const Package*>(package)->namespaces.size();
	}

	const CrpNamespace* crpPackageGetNamespace(const CrpPackage* package, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!package)
			return nullptr;
		const auto* pkg = reinterpret_cast<const Package*>(package);
		if (index >= pkg->namespaces.size())
			return nullptr;
		return reinterpret_cast<const CrpNamespace*>(&pkg->namespaces[index]);
	}

	size_t crpPackageGetEnumCount(const CrpPackage* package)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!package)
			return 0;
		return reinterpret_cast<const Package*>(package)->enums.size();
	}

	const CrpEnum* crpPackageGetEnum(const CrpPackage* package, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!package)
			return nullptr;
		const auto* pkg = reinterpret_cast<const Package*>(package);
		if (index >= pkg->enums.size())
			return nullptr;
		return reinterpret_cast<const CrpEnum*>(&pkg->enums[index]);
	}

	const char* crpClassGetName(const CrpClass* cls)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return nullptr;
		if (reinterpret_cast<const Class*>(cls)->name.empty())
			return nullptr;
		return reinterpret_cast<const Class*>(cls)->name.c_str();
	}

	const char* crpClassGetBase(const CrpClass* cls)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return nullptr;
		if (reinterpret_cast<const Class*>(cls)->base.empty())
			return nullptr;
		return reinterpret_cast<const Class*>(cls)->base.c_str();
	}

	const char* crpClassGetScope(const CrpClass* cls)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return nullptr;
		if (reinterpret_cast<const Class*>(cls)->scope.empty())
			return nullptr;
		return reinterpret_cast<const Class*>(cls)->scope.c_str();
	}

	size_t crpClassGetMethodCount(const CrpClass* cls)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return 0;
		return reinterpret_cast<const Class*>(cls)->methods.size();
	}

	const CrpClassMethod* crpClassGetMethod(const CrpClass* cls, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return nullptr;
		const auto* klass = reinterpret_cast<const Class*>(cls);
		if (index >= klass->methods.size())
			return nullptr;
		return reinterpret_cast<const CrpClassMethod*>(&klass->methods[index]);
	}

	size_t crpClassGetMemberCount(const CrpClass* cls)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return 0;
		return reinterpret_cast<const Class*>(cls)->members.size();
	}

	const CrpClassMember* crpClassGetMember(const CrpClass* cls, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return nullptr;
		const auto* klass = reinterpret_cast<const Class*>(cls);
		if (index >= klass->members.size())
			return nullptr;
		return reinterpret_cast<const CrpClassMember*>(&klass->members[index]);
	}

	int32_t crpClassIsTemplate(const CrpClass* cls)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return 0;
		return reinterpret_cast<const Class*>(cls)->isTemplateClass ? 1 : 0;
	}

	int32_t crpClassIsGeneric(const CrpClass* cls)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return 0;
		return reinterpret_cast<const Class*>(cls)->isGenericClass ? 1 : 0;
	}

	size_t crpClassGetTemplateParameterCount(const CrpClass* cls)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return 0;
		return reinterpret_cast<const Class*>(cls)->templateParameters.size();
	}

	const CrpTemplateParameter* crpClassGetTemplateParameter(const CrpClass* cls, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return nullptr;
		const auto* klass = reinterpret_cast<const Class*>(cls);
		if (index >= klass->templateParameters.size())
			return nullptr;
		return reinterpret_cast<const CrpTemplateParameter*>(&klass->templateParameters[index]);
	}

	size_t crpClassGetTemplateSpecializationCount(const CrpClass* cls)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return 0;
		return reinterpret_cast<const Class*>(cls)->templateSpecializations.size();
	}

	const char* crpClassGetTemplateSpecialization(const CrpClass* cls, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return nullptr;
		const auto* klass = reinterpret_cast<const Class*>(cls);
		if (index >= klass->templateSpecializations.size())
			return nullptr;
		if (klass->templateSpecializations[index].empty())
			return nullptr;
		return klass->templateSpecializations[index].c_str();
	}

	size_t crpClassGetGenericTypeParameterFieldCount(const CrpClass* cls)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return 0;
		return reinterpret_cast<const Class*>(cls)->genericTypeParameterFields.size();
	}

	const char* crpClassGetGenericTypeParameterField(const CrpClass* cls, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!cls)
			return nullptr;
		const auto* klass = reinterpret_cast<const Class*>(cls);
		if (index >= klass->genericTypeParameterFields.size())
			return nullptr;
		if (klass->genericTypeParameterFields[index].empty())
			return nullptr;
		return klass->genericTypeParameterFields[index].c_str();
	}

	const char* crpClassMemberGetName(const CrpClassMember* member)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!member)
			return nullptr;
		if (reinterpret_cast<const Class::Member*>(member)->name.empty())
			return nullptr;
		return reinterpret_cast<const Class::Member*>(member)->name.c_str();
	}

	const char* crpClassMemberGetType(const CrpClassMember* member)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!member)
			return nullptr;
		if (reinterpret_cast<const Class::Member*>(member)->type.empty())
			return nullptr;
		return reinterpret_cast<const Class::Member*>(member)->type.c_str();
	}

	int32_t crpClassMemberIsNative(const CrpClassMember* member)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!member)
			return 0;
		return reinterpret_cast<const Class::Member*>(member)->isNative ? 1 : 0;
	}

	const char* crpClassMethodGetName(const CrpClassMethod* method)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!method)
			return nullptr;
		if (reinterpret_cast<const Class::Method*>(method)->name.empty())
			return nullptr;
		return reinterpret_cast<const Class::Method*>(method)->name.c_str();
	}

	const char* crpClassMethodGetBase(const CrpClassMethod* method)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!method)
			return nullptr;
		if (reinterpret_cast<const Class::Method*>(method)->base.empty())
			return nullptr;
		return reinterpret_cast<const Class::Method*>(method)->base.c_str();
	}

	const char* crpClassMethodGetReturnType(const CrpClassMethod* method)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!method)
			return nullptr;
		if (reinterpret_cast<const Class::Method*>(method)->returnValue.empty())
			return nullptr;
		return reinterpret_cast<const Class::Method*>(method)->returnValue.c_str();
	}

	int32_t crpClassMethodHasCustomInvoker(const CrpClassMethod* method)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!method)
			return 0;
		return reinterpret_cast<const Class::Method*>(method)->customInvoker ? 1 : 0;
	}

	size_t crpClassMethodGetParamCount(const CrpClassMethod* method)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!method)
			return 0;
		return reinterpret_cast<const Class::Method*>(method)->params.size();
	}

	const CrpClassMethodParam* crpClassMethodGetParam(const CrpClassMethod* method, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!method)
			return nullptr;
		const auto* m = reinterpret_cast<const Class::Method*>(method);
		if (index >= m->params.size())
			return nullptr;
		return reinterpret_cast<const CrpClassMethodParam*>(&m->params[index]);
	}

	const char* crpClassMethodParamGetName(const CrpClassMethodParam* param)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!param)
			return nullptr;
		if (reinterpret_cast<const Class::Method::Params*>(param)->name.empty())
			return nullptr;
		return reinterpret_cast<const Class::Method::Params*>(param)->name.c_str();
	}

	const char* crpClassMethodParamGetType(const CrpClassMethodParam* param)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!param)
			return nullptr;
		if (reinterpret_cast<const Class::Method::Params*>(param)->type.empty())
			return nullptr;
		return reinterpret_cast<const Class::Method::Params*>(param)->type.c_str();
	}

	int32_t crpClassMethodHasDelegate(const CrpClassMethod* method)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!method)
			return 0;
		const auto* m = reinterpret_cast<const Class::Method*>(method);
		if (!m->tomlAttributes.is_table())
			return 0;
		return m->tomlAttributes.as_table().contains("Delegate") ? 1 : 0;
	}

	int32_t crpClassMethodIsBooleanDelegate(const CrpClassMethod* method)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!method)
			return 0;
		const auto* m = reinterpret_cast<const Class::Method*>(method);
		if (!m->tomlAttributes.is_table())
			return 0;
		auto it = m->tomlAttributes.as_table().find("Delegate");
		if (it == m->tomlAttributes.as_table().end())
			return 0;
		return it->second.is_boolean() ? 1 : 0;
	}

	const char* crpClassMethodGetDelegateName(const CrpClassMethod* method)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!method)
			return nullptr;
		const auto* m = reinterpret_cast<const Class::Method*>(method);
		if (!m->tomlAttributes.is_table())
			return nullptr;
		auto it = m->tomlAttributes.as_table().find("Delegate");
		if (it == m->tomlAttributes.as_table().end())
			return nullptr;
		if (!it->second.is_string())
			return nullptr;
		return it->second.as_string().c_str();
	}

	const char* crpEnumGetName(const CrpEnum* enm)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!enm)
			return nullptr;
		return reinterpret_cast<const Enum*>(enm)->name.c_str();
	}

	const char* crpEnumGetBase(const CrpEnum* enm)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!enm)
			return nullptr;
		return reinterpret_cast<const Enum*>(enm)->base.c_str();
	}

	size_t crpEnumGetElementCount(const CrpEnum* enm)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!enm)
			return 0;
		return reinterpret_cast<const Enum*>(enm)->elements.size();
	}

	const CrpEnumElement* crpEnumGetElement(const CrpEnum* enm, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!enm)
			return nullptr;
		const auto* enumType = reinterpret_cast<const Enum*>(enm);
		if (index >= enumType->elements.size())
			return nullptr;
		return reinterpret_cast<const CrpEnumElement*>(&enumType->elements[index]);
	}

	const char* crpEnumElementGetName(const CrpEnumElement* elem)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!elem)
			return nullptr;
		return reinterpret_cast<const Enum::Element*>(elem)->name.c_str();
	}

	const char* crpEnumElementGetValue(const CrpEnumElement* elem)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!elem)
			return nullptr;
		return reinterpret_cast<const Enum::Element*>(elem)->value.c_str();
	}

	const char* crpTemplateParameterGetName(const CrpTemplateParameter* param)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!param)
			return nullptr;
		return reinterpret_cast<const TemplateParameter*>(param)->name.c_str();
	}

	const char* crpNamespaceGetName(const CrpNamespace* ns)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ns)
			return nullptr;
		return reinterpret_cast<const Namespace*>(ns)->name.c_str();
	}

	size_t crpNamespaceGetClassCount(const CrpNamespace* ns)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ns)
			return 0;
		return reinterpret_cast<const Namespace*>(ns)->classes.size();
	}

	const CrpClass* crpNamespaceGetClass(const CrpNamespace* ns, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ns)
			return nullptr;
		const auto* nspace = reinterpret_cast<const Namespace*>(ns);
		if (index >= nspace->classes.size())
			return nullptr;
		return reinterpret_cast<const CrpClass*>(&nspace->classes[index]);
	}

	size_t crpNamespaceGetEnumCount(const CrpNamespace* ns)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ns)
			return 0;
		return reinterpret_cast<const Namespace*>(ns)->enums.size();
	}

	const CrpEnum* crpNamespaceGetEnum(const CrpNamespace* ns, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ns)
			return nullptr;
		const auto* nspace = reinterpret_cast<const Namespace*>(ns);
		if (index >= nspace->enums.size())
			return nullptr;
		return reinterpret_cast<const CrpEnum*>(&nspace->enums[index]);
	}

	size_t crpNamespaceGetNamespaceCount(const CrpNamespace* ns)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ns)
			return 0;
		return reinterpret_cast<const Namespace*>(ns)->namespaces.size();
	}

	const CrpNamespace* crpNamespaceGetNamespace(const CrpNamespace* ns, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ns)
			return nullptr;
		const auto* nspace = reinterpret_cast<const Namespace*>(ns);
		if (index >= nspace->namespaces.size())
			return nullptr;
		return reinterpret_cast<const CrpNamespace*>(&nspace->namespaces[index]);
	}

	void crpGenerationContextWrite(CrpGenerationContext* ctx, const char* format, ...)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ctx || !format)
		{
			CCT_ASSERT_FALSE("crpGenerationContextWrite: ctx or format is null");
			return;
		}

		auto* context = reinterpret_cast<cct::GenerationContext*>(ctx);

		va_list args;
		va_start(args, format);

		va_list argsCopy;
		va_copy(argsCopy, args);
		int size = std::vsnprintf(nullptr, 0, format, argsCopy);
		va_end(argsCopy);

		if (size < 0)
		{
			va_end(args);
			return;
		}

		std::string buffer(size + 1, '\0');
		std::vsnprintf(buffer.data(), buffer.size(), format, args);
		va_end(args);

		buffer.resize(size);
		context->Write(buffer);
	}

	void crpGenerationContextNewLine(CrpGenerationContext* ctx)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ctx)
		{
			CCT_ASSERT_FALSE("crpGenerationContextNewLine: ctx is null");
			return;
		}

		auto* context = reinterpret_cast<cct::GenerationContext*>(ctx);
		context->NewLine();
	}

	void crpGenerationContextEnterScope(CrpGenerationContext* ctx)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ctx)
		{
			CCT_ASSERT_FALSE("crpGenerationContextEnterScope: ctx is null");
			return;
		}

		auto* context = reinterpret_cast<cct::GenerationContext*>(ctx);
		context->EnterScope();
	}

	void crpGenerationContextLeaveScope(CrpGenerationContext* ctx, const char* suffix)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ctx)
		{
			CCT_ASSERT_FALSE("crpGenerationContextLeaveScope: ctx is null");
			return;
		}

		auto* context = reinterpret_cast<cct::GenerationContext*>(ctx);
		context->LeaveScope(suffix ? suffix : "");
	}

	const CrpPackage* crpGenerationContextGetPackage(CrpGenerationContext* ctx)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ctx)
		{
			CCT_ASSERT_FALSE("crpGenerationContextGetPackage: ctx is null");
			return nullptr;
		}
		auto* context = reinterpret_cast<cct::GenerationContext*>(ctx);
		return reinterpret_cast<const CrpPackage*>(context->package);
	}

	const CrpClass* crpGenerationContextGetClass(CrpGenerationContext* ctx)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ctx)
		{
			CCT_ASSERT_FALSE("crpGenerationContextGetClass: ctx is null");
			return nullptr;
		}
		auto* context = reinterpret_cast<cct::GenerationContext*>(ctx);
		return reinterpret_cast<const CrpClass*>(context->currentClass);
	}

	const CrpNamespace* crpGenerationContextGetNamespace(CrpGenerationContext* ctx)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ctx)
		{
			CCT_ASSERT_FALSE("crpGenerationContextGetNamespace: ctx is null");
			return nullptr;
		}
		auto* context = reinterpret_cast<cct::GenerationContext*>(ctx);
		return reinterpret_cast<const CrpNamespace*>(context->currentNamespace);
	}

	const char* crpGenerationContextGetNamespacePath(CrpGenerationContext* ctx)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ctx)
		{
			CCT_ASSERT_FALSE("crpGenerationContextGetNamespacePath: ctx is null");
			return nullptr;
		}
		auto* context = reinterpret_cast<cct::GenerationContext*>(ctx);
		return context->namespacePath;
	}

	void* crpGenerationContextGetPrivateData(CrpGenerationContext* ctx)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ctx)
			return nullptr;
		auto* context = reinterpret_cast<cct::GenerationContext*>(ctx);
		return context->pluginPrivateData;
	}
	size_t crpGenerationContextGetHeaderCount(CrpGenerationContext* ctx)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ctx)
			return 0;
		auto* context = reinterpret_cast<cct::GenerationContext*>(ctx);
		return context->headerCount;
	}

	const char* crpGenerationContextGetHeader(CrpGenerationContext* ctx, size_t index)
	{
		CCT_AUTO_PROFILER_SCOPE();
		if (!ctx)
			return nullptr;
		auto* context = reinterpret_cast<cct::GenerationContext*>(ctx);
		if (index >= context->headerCount || !context->headers)
			return nullptr;
		return context->headers[index];
	}
} // extern "C"
