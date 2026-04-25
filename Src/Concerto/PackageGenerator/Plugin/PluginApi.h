//
// Created by arthur
//

#ifndef CONCERTO_PKGGENERATOR_PLUGINAPI_H
#define CONCERTO_PKGGENERATOR_PLUGINAPI_H

#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
#define CRP_PLUGIN_EXPORT __declspec(dllexport)
#define CRP_PLUGIN_IMPORT __declspec(dllimport)
#else
#define CRP_PLUGIN_EXPORT __attribute__((visibility("default")))
#define CRP_PLUGIN_IMPORT
#endif

// API functions export/import
#ifdef CRP_PLUGIN_API_BUILD
#define CRP_PLUGIN_API CRP_PLUGIN_EXPORT
#else
#define CRP_PLUGIN_API CRP_PLUGIN_IMPORT
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#define CRP_PLUGIN_API_VERSION 1
	typedef struct CrpPackage CrpPackage;
	typedef struct CrpGenerationContext CrpGenerationContext;
	typedef struct CrpNamespace CrpNamespace;
	typedef struct CrpClass CrpClass;
	typedef struct CrpClassMember CrpClassMember;
	typedef struct CrpClassMethod CrpClassMethod;
	typedef struct CrpEnum CrpEnum;
	typedef struct CrpEnumElement CrpEnumElement;
	typedef struct CrpTemplateParameter CrpTemplateParameter;
	typedef struct CrpClassMethodParam CrpClassMethodParam;
	typedef struct CrpTomlAttributes CrpTomlAttributes;

	typedef struct CrpPluginInfo
	{
		const char* name;
		const char* version;
		const char* author;
		const char* description;
		const char* outputFileExtension; // ".gen.hpp", ".gen.cpp", etc.
	} CrpPluginInfo;

	/**
	 * Function table exported by each plugin.
	 * All function pointers can be NULL if the plugin does not implement them.
	 * The loader will check for NULL before calling.
	 */
	typedef struct CrpPluginFunctionTable
	{
		uint32_t apiVersion;

		/**
		 * Get plugin information.
		 * @param outInfo Pointer to fill with plugin info
		 */
		void (*GetInfo)(CrpPluginInfo* outInfo);

		/**
		 * Get plugin priority for ordering.
		 * Higher priority plugins run first.
		 * @return Priority value (default: 0)
		 */
		int32_t (*GetPriority)(void);

		/**
		 * Called when the plugin is loaded.
		 * @param outPrivateData Optional pointer to store plugin-specific data (can be left NULL).
		 * @return 1 on success, 0 on failure.
		 */
		int32_t (*OnLoad)(void** outPrivateData);
		void (*OnUnload)(void* privateData);

		/**
		 * Transform the package before generation.
		 * @param package Package to transform
		 */
		void (*TransformPackage)(CrpPackage* package);

		void (*BeforePackageGeneration)(const CrpPackage* package, CrpGenerationContext* ctx);
		void (*OnPackageGeneration)(const CrpPackage* package, CrpGenerationContext* ctx);
		void (*AfterPackageGeneration)(const CrpPackage* package, CrpGenerationContext* ctx);

		void (*BeforeNamespaceGeneration)(const CrpNamespace* ns, CrpGenerationContext* ctx);
		void (*OnNamespaceGeneration)(const CrpNamespace* ns, CrpGenerationContext* ctx);
		void (*AfterNamespaceGeneration)(const CrpNamespace* ns, CrpGenerationContext* ctx);

		void (*BeforeClassGeneration)(const CrpClass* cls, CrpGenerationContext* ctx);
		void (*OnClassGeneration)(const CrpClass* cls, CrpGenerationContext* ctx);
		void (*AfterClassGeneration)(const CrpClass* cls, CrpGenerationContext* ctx);

		void (*BeforeMemberGeneration)(const CrpClassMember* member, CrpGenerationContext* ctx);
		void (*OnMemberGeneration)(const CrpClassMember* member, CrpGenerationContext* ctx);
		void (*AfterMemberGeneration)(const CrpClassMember* member, CrpGenerationContext* ctx);

		void (*BeforeMethodGeneration)(const CrpClassMethod* method, CrpGenerationContext* ctx);
		void (*OnMethodGeneration)(const CrpClassMethod* method, CrpGenerationContext* ctx);
		void (*AfterMethodGeneration)(const CrpClassMethod* method, CrpGenerationContext* ctx);

		void (*BeforeEnumGeneration)(const CrpEnum* enumeration, CrpGenerationContext* ctx);
		void (*OnEnumGeneration)(const CrpEnum* enumeration, CrpGenerationContext* ctx);
		void (*AfterEnumGeneration)(const CrpEnum* enumeration, CrpGenerationContext* ctx);

		void (*BeforeEnumElementGeneration)(const CrpEnumElement* element, CrpGenerationContext* ctx);
		void (*OnEnumElementGeneration)(const CrpEnumElement* element, CrpGenerationContext* ctx);
		void (*AfterEnumElementGeneration)(const CrpEnumElement* element, CrpGenerationContext* ctx);

		void (*BeforeTemplateClassGeneration)(const CrpClass* cls, CrpGenerationContext* ctx);
		void (*OnTemplateClassGeneration)(const CrpClass* cls, CrpGenerationContext* ctx);
		void (*AfterTemplateClassGeneration)(const CrpClass* cls, CrpGenerationContext* ctx);

		void (*BeforeGenericClassGeneration)(const CrpClass* cls, CrpGenerationContext* ctx);
		void (*OnGenericClassGeneration)(const CrpClass* cls, CrpGenerationContext* ctx);
		void (*AfterGenericClassGeneration)(const CrpClass* cls, CrpGenerationContext* ctx);

		void (*BeforeTemplateSpecializationGeneration)(const CrpClass* cls, const char* specialization, CrpGenerationContext* ctx);
		void (*OnTemplateSpecializationGeneration)(const CrpClass* cls, const char* specialization, CrpGenerationContext* ctx);
		void (*AfterTemplateSpecializationGeneration)(const CrpClass* cls, const char* specialization, CrpGenerationContext* ctx);

		int32_t (*HandleAnnotation)(
			const char* annotationType,
			const char* entityName,
			const CrpTomlAttributes* attributes,
			CrpGenerationContext* ctx);

		/**
		 * Generate files for this plugin (optional).
		 * Plugin uses crpGenerationContextWrite() to write to ctx->outputStream.
		 * The host will write the stream contents to the appropriate file.
		 * @param package Package to generate files for
		 * @param ctx Generation context with outputStream
		 * @param outputFolder Folder where files should be generated
		 * @param headers Array of header file paths to include (can be NULL)
		 * @param headerCount Number of headers
		 * @return 1 if success, 0 if failure
		 */
		int32_t (*GenerateFile)(
			const CrpPackage* package,
			CrpGenerationContext* ctx,
			const char* outputFolder,
			const char** headers,
			size_t headerCount);

	} CrpPluginFunctionTable;

	typedef const CrpPluginFunctionTable* (*CrpGetPluginFunctionTableFunc)(void);

#define CRP_PLUGIN_ENTRY_POINT_NAME "crpGetPluginFunctionTable"

#define CRP_DECLARE_PLUGIN_ENTRY(table)                                             \
	CRP_PLUGIN_EXPORT const CrpPluginFunctionTable* crpGetPluginFunctionTable(void) \
	{                                                                               \
		return &(table);                                                            \
	}

	CRP_PLUGIN_API const char* crpPackageGetName(const CrpPackage* package);
	CRP_PLUGIN_API const char* crpPackageGetVersion(const CrpPackage* package);
	CRP_PLUGIN_API const char* crpPackageGetDescription(const CrpPackage* package);
	CRP_PLUGIN_API size_t crpPackageGetClassCount(const CrpPackage* package);
	CRP_PLUGIN_API const CrpClass* crpPackageGetClass(const CrpPackage* package, size_t index);
	CRP_PLUGIN_API size_t crpPackageGetNamespaceCount(const CrpPackage* package);
	CRP_PLUGIN_API const CrpNamespace* crpPackageGetNamespace(const CrpPackage* package, size_t index);
	CRP_PLUGIN_API size_t crpPackageGetEnumCount(const CrpPackage* package);
	CRP_PLUGIN_API const CrpEnum* crpPackageGetEnum(const CrpPackage* package, size_t index);

	CRP_PLUGIN_API const char* crpClassGetName(const CrpClass* cls);
	CRP_PLUGIN_API const char* crpClassGetBase(const CrpClass* cls);
	CRP_PLUGIN_API const char* crpClassGetScope(const CrpClass* cls);
	CRP_PLUGIN_API size_t crpClassGetMethodCount(const CrpClass* cls);
	CRP_PLUGIN_API const CrpClassMethod* crpClassGetMethod(const CrpClass* cls, size_t index);
	CRP_PLUGIN_API size_t crpClassGetMemberCount(const CrpClass* cls);
	CRP_PLUGIN_API const CrpClassMember* crpClassGetMember(const CrpClass* cls, size_t index);
	CRP_PLUGIN_API int32_t crpClassIsTemplate(const CrpClass* cls);
	CRP_PLUGIN_API int32_t crpClassIsGeneric(const CrpClass* cls);
	CRP_PLUGIN_API size_t crpClassGetTemplateParameterCount(const CrpClass* cls);
	CRP_PLUGIN_API const CrpTemplateParameter* crpClassGetTemplateParameter(const CrpClass* cls, size_t index);
	CRP_PLUGIN_API size_t crpClassGetTemplateSpecializationCount(const CrpClass* cls);
	CRP_PLUGIN_API const char* crpClassGetTemplateSpecialization(const CrpClass* cls, size_t index);

	CRP_PLUGIN_API size_t crpClassGetGenericTypeParameterFieldCount(const CrpClass* cls);
	CRP_PLUGIN_API const char* crpClassGetGenericTypeParameterField(const CrpClass* cls, size_t index);

	CRP_PLUGIN_API const char* crpClassMemberGetName(const CrpClassMember* member);
	CRP_PLUGIN_API const char* crpClassMemberGetType(const CrpClassMember* member);
	CRP_PLUGIN_API int32_t crpClassMemberIsNative(const CrpClassMember* member);

	CRP_PLUGIN_API const char* crpClassMethodGetName(const CrpClassMethod* method);
	CRP_PLUGIN_API const char* crpClassMethodGetBase(const CrpClassMethod* method);
	CRP_PLUGIN_API const char* crpClassMethodGetReturnType(const CrpClassMethod* method);
	CRP_PLUGIN_API int32_t crpClassMethodHasCustomInvoker(const CrpClassMethod* method);
	CRP_PLUGIN_API size_t crpClassMethodGetParamCount(const CrpClassMethod* method);
	CRP_PLUGIN_API const CrpClassMethodParam* crpClassMethodGetParam(const CrpClassMethod* method, size_t index);
	CRP_PLUGIN_API const char* crpClassMethodParamGetName(const CrpClassMethodParam* param);
	CRP_PLUGIN_API const char* crpClassMethodParamGetType(const CrpClassMethodParam* param);
	CRP_PLUGIN_API int32_t crpClassMethodHasDelegate(const CrpClassMethod* method);
	CRP_PLUGIN_API int32_t crpClassMethodIsBooleanDelegate(const CrpClassMethod* method);
	CRP_PLUGIN_API const char* crpClassMethodGetDelegateName(const CrpClassMethod* method);

	CRP_PLUGIN_API const char* crpEnumGetName(const CrpEnum* enm);
	CRP_PLUGIN_API const char* crpEnumGetBase(const CrpEnum* enm);
	CRP_PLUGIN_API size_t crpEnumGetElementCount(const CrpEnum* enm);
	CRP_PLUGIN_API const CrpEnumElement* crpEnumGetElement(const CrpEnum* enm, size_t index);

	CRP_PLUGIN_API const char* crpEnumElementGetName(const CrpEnumElement* elem);
	CRP_PLUGIN_API const char* crpEnumElementGetValue(const CrpEnumElement* elem);

	CRP_PLUGIN_API const char* crpTemplateParameterGetName(const CrpTemplateParameter* param);

	CRP_PLUGIN_API const char* crpNamespaceGetName(const CrpNamespace* ns);
	CRP_PLUGIN_API size_t crpNamespaceGetClassCount(const CrpNamespace* ns);
	CRP_PLUGIN_API const CrpClass* crpNamespaceGetClass(const CrpNamespace* ns, size_t index);
	CRP_PLUGIN_API size_t crpNamespaceGetEnumCount(const CrpNamespace* ns);
	CRP_PLUGIN_API const CrpEnum* crpNamespaceGetEnum(const CrpNamespace* ns, size_t index);
	CRP_PLUGIN_API size_t crpNamespaceGetNamespaceCount(const CrpNamespace* ns);
	CRP_PLUGIN_API const CrpNamespace* crpNamespaceGetNamespace(const CrpNamespace* ns, size_t index);

	CRP_PLUGIN_API void crpGenerationContextWrite(CrpGenerationContext* ctx, const char* format, ...);
	CRP_PLUGIN_API void crpGenerationContextNewLine(CrpGenerationContext* ctx);
	CRP_PLUGIN_API void crpGenerationContextEnterScope(CrpGenerationContext* ctx);
	CRP_PLUGIN_API void crpGenerationContextLeaveScope(CrpGenerationContext* ctx, const char* suffix);
	CRP_PLUGIN_API const CrpPackage* crpGenerationContextGetPackage(CrpGenerationContext* ctx);
	CRP_PLUGIN_API const CrpClass* crpGenerationContextGetClass(CrpGenerationContext* ctx);
	CRP_PLUGIN_API const CrpNamespace* crpGenerationContextGetNamespace(CrpGenerationContext* ctx);
	CRP_PLUGIN_API const char* crpGenerationContextGetNamespacePath(CrpGenerationContext* ctx);
	CRP_PLUGIN_API void* crpGenerationContextGetPrivateData(CrpGenerationContext* ctx);
	CRP_PLUGIN_API size_t crpGenerationContextGetHeaderCount(CrpGenerationContext* ctx);
	CRP_PLUGIN_API const char* crpGenerationContextGetHeader(CrpGenerationContext* ctx, size_t index);

#ifdef __cplusplus
}
#endif

#endif // CONCERTO_PKGGENERATOR_PLUGINAPI_H
