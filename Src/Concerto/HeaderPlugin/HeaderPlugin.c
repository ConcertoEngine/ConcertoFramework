//
// HeaderPlugin.c - Generates .gen.hpp header files
//

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Concerto/PackageGenerator/Plugin/PluginApi.h"

typedef struct HeaderPluginData
{
	int reserved;
} HeaderPluginData;

// Plugin info
static void GetInfo(CrpPluginInfo* outInfo)
{
	outInfo->name = "HeaderPlugin";
	outInfo->version = "1.0.0";
	outInfo->author = "Concerto";
	outInfo->description = "Generates .gen.hpp header files for reflection";
	outInfo->outputFileExtension = ".gen.hpp";
}

static int32_t OnLoad(void** outPrivateData)
{
	HeaderPluginData* data = (HeaderPluginData*)calloc(1, sizeof(HeaderPluginData));
	if (!data)
		return 0;
	if (outPrivateData)
		*outPrivateData = data;
	return 1;
}

static void OnUnload(void* privateData)
{
	free(privateData);
}

static void ToUpper(const char* src, char* dest, size_t destSize)
{
	size_t i = 0;
	while (src[i] && i < destSize - 1)
	{
		if (src[i] >= 'a' && src[i] <= 'z')
			dest[i] = src[i] - 32;
		else
			dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

static void Capitalize(const char* src, char* dest, size_t destSize)
{
	if (destSize == 0)
		return;

	if (src[0] >= 'a' && src[0] <= 'z')
		dest[0] = src[0] - 32;
	else
		dest[0] = src[0];

	size_t i = 1;
	while (src[i] && i < destSize - 1)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

static void GetApiMacro(CrpGenerationContext* ctx, char* upperName, size_t upperNameSize)
{
	const CrpPackage* package = crpGenerationContextGetPackage(ctx);
	const char* packageName = crpPackageGetName(package);
	ToUpper(packageName, upperName, upperNameSize);
}

// --- Hooks ---

static void BeforePackageGeneration(const CrpPackage* package, CrpGenerationContext* ctx)
{
	const char* packageName = crpPackageGetName(package);

	char upperName[256];
	ToUpper(packageName, upperName, sizeof(upperName));

	// Header comment and pragma once
	crpGenerationContextWrite(ctx, "//This file was automatically generated, do not edit");
	crpGenerationContextWrite(ctx, "#pragma once");
	crpGenerationContextNewLine(ctx);

	crpGenerationContextWrite(ctx, "#include <string_view>");
	crpGenerationContextWrite(ctx, "#include <Concerto/Reflection/Defines.hpp>");
	crpGenerationContextWrite(ctx, "#include <Concerto/Reflection/Class/Class.hpp>");
	crpGenerationContextWrite(ctx, "#include <Concerto/Reflection/Object/Object.refl.hpp>");
	crpGenerationContextWrite(ctx, "#include <Concerto/Reflection/MemberVariable/MemberVariable.hpp>");
	crpGenerationContextWrite(ctx, "#include <Concerto/Reflection/Method/Method.hpp>");
	crpGenerationContextWrite(ctx, "#include <Concerto/Reflection/Package/Package.hpp>");
	crpGenerationContextWrite(ctx, "#include <Concerto/Reflection/TemplateClass/TemplateClass.hpp>");
	crpGenerationContextWrite(ctx, "#include <Concerto/Reflection/GenericClass/GenericClass.hpp>");

	// API macro definition
	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "#ifdef %sPACKAGE_STATIC", upperName);
	crpGenerationContextWrite(ctx, "\t#define %sPACKAGE_API", upperName);
	crpGenerationContextWrite(ctx, "#else");
	crpGenerationContextWrite(ctx, "\t#ifdef %sPACKAGE_BUILD", upperName);
	crpGenerationContextWrite(ctx, "\t\t#define %sPACKAGE_API CCT_EXPORT", upperName);
	crpGenerationContextWrite(ctx, "\t#else");
	crpGenerationContextWrite(ctx, "\t\t#define %sPACKAGE_API CCT_IMPORT", upperName);
	crpGenerationContextWrite(ctx, "\t#endif");
	crpGenerationContextWrite(ctx, "#endif");
	crpGenerationContextNewLine(ctx);
}

static void AfterPackageGeneration(const CrpPackage* package, CrpGenerationContext* ctx)
{
	const char* packageName = crpPackageGetName(package);

	char upperName[256];
	ToUpper(packageName, upperName, sizeof(upperName));

	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "%sPACKAGE_API std::unique_ptr<cct::refl::Package> Create%sPackage(cct::Logger* logger = nullptr);",
							  upperName, packageName);
}

static void BeforeNamespaceGeneration(const CrpNamespace* ns, CrpGenerationContext* ctx)
{
	const char* namespaceName = crpNamespaceGetName(ns);
	crpGenerationContextWrite(ctx, "namespace %s", namespaceName);
	crpGenerationContextEnterScope(ctx);
}

static void AfterNamespaceGeneration(const CrpNamespace* ns, CrpGenerationContext* ctx)
{
	crpGenerationContextNewLine(ctx);
	crpGenerationContextLeaveScope(ctx, NULL);
}

static void BeforeEnumGeneration(const CrpEnum* enm, CrpGenerationContext* ctx)
{
	const char* enumName = crpEnumGetName(enm);
	const char* enumBase = crpEnumGetBase(enm);

	char upperName[256];
	GetApiMacro(ctx, upperName, sizeof(upperName));

	char capitalizedName[256];
	Capitalize(enumName, capitalizedName, sizeof(capitalizedName));

	crpGenerationContextWrite(ctx, "enum class %s : %s;", enumName, enumBase);
	crpGenerationContextWrite(ctx, "%sPACKAGE_API std::string_view %sToString(%s value);", upperName, capitalizedName, enumName);
	crpGenerationContextWrite(ctx, "%sPACKAGE_API %s %sFromString(std::string_view str);", upperName, enumName, capitalizedName);
}

static int32_t GetPriority(void)
{
	// Header plugin should run with higher priority so .gen.hpp is generated first
	return 100;
}

// Function table
static const CrpPluginFunctionTable g_pluginTable = {
	.apiVersion = CRP_PLUGIN_API_VERSION,
	.GetInfo = GetInfo,
	.GetPriority = GetPriority,
	.OnLoad = OnLoad,
	.OnUnload = OnUnload,
	.TransformPackage = NULL,

	.BeforePackageGeneration = BeforePackageGeneration,
	.OnPackageGeneration = NULL,
	.AfterPackageGeneration = AfterPackageGeneration,

	.BeforeNamespaceGeneration = BeforeNamespaceGeneration,
	.OnNamespaceGeneration = NULL,
	.AfterNamespaceGeneration = AfterNamespaceGeneration,

	.BeforeClassGeneration = NULL,
	.OnClassGeneration = NULL,
	.AfterClassGeneration = NULL,

	.BeforeMemberGeneration = NULL,
	.OnMemberGeneration = NULL,
	.AfterMemberGeneration = NULL,

	.BeforeMethodGeneration = NULL,
	.OnMethodGeneration = NULL,
	.AfterMethodGeneration = NULL,

	.BeforeEnumGeneration = BeforeEnumGeneration,
	.OnEnumGeneration = NULL,
	.AfterEnumGeneration = NULL,

	.BeforeEnumElementGeneration = NULL,
	.OnEnumElementGeneration = NULL,
	.AfterEnumElementGeneration = NULL,

	.BeforeTemplateClassGeneration = NULL,
	.OnTemplateClassGeneration = NULL,
	.AfterTemplateClassGeneration = NULL,

	.BeforeGenericClassGeneration = NULL,
	.OnGenericClassGeneration = NULL,
	.AfterGenericClassGeneration = NULL,

	.BeforeTemplateSpecializationGeneration = NULL,
	.OnTemplateSpecializationGeneration = NULL,
	.AfterTemplateSpecializationGeneration = NULL,

	.HandleAnnotation = NULL,
	.GenerateFile = NULL,
};

// Entry point
CRP_DECLARE_PLUGIN_ENTRY(g_pluginTable)
