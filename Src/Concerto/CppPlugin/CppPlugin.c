//
// CppPlugin.c - Generates .gen.cpp implementation files
//

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Concerto/PackageGenerator/Plugin/PluginApi.h"

typedef struct CppPluginData
{
	int reserved;
} CppPluginData;

static void GetInfo(CrpPluginInfo* outInfo)
{
	outInfo->name = "CppPlugin";
	outInfo->version = "1.0.0";
	outInfo->author = "Concerto";
	outInfo->description = "Generates .gen.cpp implementation files for reflection";
	outInfo->outputFileExtension = ".gen.cpp";
}

static int32_t OnLoad(void** outPrivateData)
{
	CppPluginData* data = (CppPluginData*)calloc(1, sizeof(CppPluginData));
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

static int IsAsciiAlphaNum(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

static void SanitizeIdentifier(const char* src, char* dest, size_t destSize)
{
	if (!dest || destSize == 0)
		return;

	if (!src || src[0] == '\0')
	{
		if (destSize >= 2)
		{
			dest[0] = '_';
			dest[1] = '\0';
		}
		else
		{
			dest[0] = '\0';
		}
		return;
	}

	size_t i = 0;
	while (src[i] && i < destSize - 1)
	{
		const char c = src[i];
		dest[i] = (IsAsciiAlphaNum(c) || c == '_') ? c : '_';
		++i;
	}
	dest[i] = '\0';

	if (dest[0] >= '0' && dest[0] <= '9')
	{
		if (i + 1 < destSize)
		{
			memmove(dest + 1, dest, i + 1);
			dest[0] = 'T';
		}
		else
		{
			dest[0] = 'T';
		}
	}
}

static void SanitizeSpecialization(const char* src, char* dest, size_t destSize)
{
	size_t i = 0;
	while (src[i] && i < destSize - 1)
	{
		char c = src[i];
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
			dest[i] = c;
		else
			dest[i] = '_';
		i++;
	}
	dest[i] = '\0';
}

static int AppendToBuffer(char* dest, size_t destSize, const char* src)
{
	if (!dest || !src || destSize == 0)
		return 0;

	const size_t destLen = strlen(dest);
	const size_t srcLen = strlen(src);
	if (destLen + srcLen >= destSize)
		return 0;

	memcpy(dest + destLen, src, srcLen + 1);
	return 1;
}

static const char* StripConst(const char* type, char* buf, size_t bufSize)
{
	if (strncmp(type, "const ", 6) == 0)
	{
		size_t len = strlen(type) - 6;
		if (len >= bufSize)
			len = bufSize - 1;
		memcpy(buf, type + 6, len);
		buf[len] = '\0';
		return buf;
	}
	return type;
}

static void BeforeMethodGeneration(const CrpClassMethod* method, CrpGenerationContext* ctx)
{
	const CrpClass* cls = crpGenerationContextGetClass(ctx);
	const char* className = crpClassGetName(cls);
	const char* methodName = crpClassMethodGetName(method);
	const char* base = crpClassMethodGetBase(method);
	size_t paramCount = crpClassMethodGetParamCount(method);

	const char* baseClass = (base && base[0] != '\0') ? base : "cct::refl::Method";

	crpGenerationContextWrite(ctx, "class %s%sMethod : public %s", className, methodName, baseClass);
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "public:");
		crpGenerationContextWrite(ctx, "using BaseClass = %s;", baseClass);
		crpGenerationContextWrite(ctx, "using BaseClass::BaseClass;");
		crpGenerationContextNewLine(ctx);

		crpGenerationContextWrite(ctx, "void Initialize() override");
		crpGenerationContextEnterScope(ctx);
		crpGenerationContextLeaveScope(ctx, NULL);
		crpGenerationContextNewLine(ctx);

		crpGenerationContextWrite(ctx, "cct::Result<cct::Any, std::string> Invoke(cct::refl::Object& self, std::span<cct::Any> parameters) const override");
		crpGenerationContextEnterScope(ctx);
		{
			crpGenerationContextWrite(ctx, "if (parameters.size() != %zu)", paramCount);
			crpGenerationContextEnterScope(ctx);
			{
				crpGenerationContextWrite(ctx, "CCT_ASSERT_FALSE(\"Invalid parameters size\");");
				crpGenerationContextWrite(ctx, "return {\"Invalid parameters size\"s};");
			}
			crpGenerationContextLeaveScope(ctx, NULL);

			for (size_t i = 0; i < paramCount; ++i)
			{
				const CrpClassMethodParam* param = crpClassMethodGetParam(method, i);
				const char* paramType = crpClassMethodParamGetType(param);
				const char* paramName = crpClassMethodParamGetName(param);

				char strippedType[512];
				const char* usedType = StripConst(paramType, strippedType, sizeof(strippedType));

				crpGenerationContextWrite(ctx, "using Param%zu = %s;", i, usedType);
				crpGenerationContextWrite(ctx, "if (parameters[%zu].Is<Param%zu>() == false)", i, i);
				crpGenerationContextEnterScope(ctx);
				{
					crpGenerationContextWrite(ctx, "CCT_ASSERT_FALSE(\"Expected '%s' in argument %zu\");", paramType, i);
					crpGenerationContextWrite(ctx, "return {\"Expected '%s' in argument %zu\"s};", paramType, i);
				}
				crpGenerationContextLeaveScope(ctx, NULL);
				crpGenerationContextWrite(ctx, "Param%zu %s = parameters[%zu].As<Param%zu>();", i, paramName, i, i);
				crpGenerationContextNewLine(ctx);
			}

			crpGenerationContextNewLine(ctx);
		}
	}
}

static void BeforeEnumGeneration(const CrpEnum* enumeration, CrpGenerationContext* ctx)
{
	const char* enumName = crpEnumGetName(enumeration);

	crpGenerationContextWrite(ctx, "class Internal%sEnumerationClass : public cct::refl::EnumerationClass", enumName);
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "public:");
		crpGenerationContextWrite(ctx, "Internal%sEnumerationClass()", enumName);
		crpGenerationContextWrite(ctx, ": cct::refl::EnumerationClass(nullptr, \"%s\"s)", enumName);
		crpGenerationContextEnterScope(ctx);
		crpGenerationContextLeaveScope(ctx, NULL);

		crpGenerationContextNewLine(ctx);
		crpGenerationContextWrite(ctx, "~Internal%sEnumerationClass() override", enumName);
		crpGenerationContextEnterScope(ctx);
		crpGenerationContextLeaveScope(ctx, NULL);

		crpGenerationContextNewLine(ctx);
		crpGenerationContextWrite(ctx, "void Initialize() override");
		crpGenerationContextEnterScope(ctx);
	}
}

static void OnEnumElementGeneration(const CrpEnumElement* element, CrpGenerationContext* ctx)
{
	const char* elemName = crpEnumElementGetName(element);
	const char* elemValue = crpEnumElementGetValue(element);
	crpGenerationContextWrite(ctx, "AddEnumValue(\"%s\", %s);", elemName, elemValue);
}

static void AfterEnumGeneration(const CrpEnum* enumeration, CrpGenerationContext* ctx)
{
	const char* enumName = crpEnumGetName(enumeration);
	const char* nsPath = crpGenerationContextGetNamespacePath(ctx);

	crpGenerationContextLeaveScope(ctx, NULL);

	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "cct::refl::Object* GetMemberVariable(std::size_t, const cct::refl::Object&) const override");
	crpGenerationContextEnterScope(ctx);
	crpGenerationContextWrite(ctx, "return nullptr;");
	crpGenerationContextLeaveScope(ctx, NULL);

	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "void* GetNativeMemberVariable(std::size_t, const cct::refl::Object&) const override");
	crpGenerationContextEnterScope(ctx);
	crpGenerationContextWrite(ctx, "return nullptr;");
	crpGenerationContextLeaveScope(ctx, NULL);

	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "std::unique_ptr<cct::refl::Object> CreateDefaultObject() const override");
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "auto object = std::unique_ptr<cct::refl::Object>(new cct::refl::Enumeration);");
		crpGenerationContextWrite(ctx, "if (object)");
		crpGenerationContextEnterScope(ctx);
		{
			crpGenerationContextWrite(ctx, "object->SetDynamicClass(this);");
			crpGenerationContextWrite(ctx, "object->InitializeMemberVariables();");
			for (size_t i = 0; i < crpEnumGetElementCount(enumeration); ++i)
			{
				const CrpEnumElement* element = crpEnumGetElement(enumeration, i);
				const char* elemName = crpEnumElementGetName(element);
				const char* elemValue = crpEnumElementGetValue(element);
				crpGenerationContextWrite(ctx, "AddDynamicEnumValue(static_cast<cct::refl::Enumeration&>(*object.get()), \"%s\", %s);", elemName, elemValue);
			}
		}
		crpGenerationContextLeaveScope(ctx, NULL);
		crpGenerationContextWrite(ctx, "return object;");
	}
	crpGenerationContextLeaveScope(ctx, NULL);
	crpGenerationContextLeaveScope(ctx, ";");
}

static void BeforeClassGeneration(const CrpClass* cls, CrpGenerationContext* ctx)
{
	const char* className = crpClassGetName(cls);
	const char* baseClass = crpClassGetBase(cls);
	const char* nsPath = crpGenerationContextGetNamespacePath(ctx);

	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "class Internal%sClass : public cct::refl::Class", className);
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "public:");
		crpGenerationContextWrite(ctx, "Internal%sClass() : cct::refl::Class(nullptr, \"%s\"s, nullptr)", className, className);
		crpGenerationContextEnterScope(ctx);
		{
			crpGenerationContextWrite(ctx, "if (%s::m_class != nullptr)", className);
			crpGenerationContextEnterScope(ctx);
			{
				crpGenerationContextWrite(ctx, "CCT_ASSERT_FALSE(\"Class already created\");");
				crpGenerationContextWrite(ctx, "return;");
			}
			crpGenerationContextLeaveScope(ctx, NULL);
			crpGenerationContextWrite(ctx, "%s::m_class = this;", className);
		}
		crpGenerationContextLeaveScope(ctx, NULL);

		crpGenerationContextWrite(ctx, "~Internal%sClass() override", className);
		crpGenerationContextEnterScope(ctx);
		crpGenerationContextWrite(ctx, "%s::m_class = nullptr;", className);
		crpGenerationContextLeaveScope(ctx, NULL);

		crpGenerationContextNewLine(ctx);
		crpGenerationContextWrite(ctx, "void Initialize() override");
		crpGenerationContextEnterScope(ctx);
		{
			if (nsPath && nsPath[0] != '\0')
				crpGenerationContextWrite(ctx, "SetNamespace(GlobalNamespace::Get().GetNamespaceByName(\"%s\"sv));", nsPath);

			if (baseClass && baseClass[0] != '\0')
			{
				crpGenerationContextWrite(ctx, "const Class* baseClass = GetClassByName(\"%s\"sv);", baseClass);
				crpGenerationContextWrite(ctx, "if (baseClass == nullptr)");
				crpGenerationContextEnterScope(ctx);
				{
					if (nsPath && nsPath[0] != '\0')
						crpGenerationContextWrite(ctx, "baseClass = GetClassByName(\"%s::%s\"sv);", nsPath, baseClass);
				}
				crpGenerationContextLeaveScope(ctx, NULL);
				crpGenerationContextWrite(ctx, "CCT_ASSERT(baseClass != nullptr, \"Could not find class '%s'\");", baseClass);
				crpGenerationContextWrite(ctx, "SetBaseClass(baseClass);");
			}

			crpGenerationContextNewLine(ctx);
		}
	}
}

static void OnMemberGeneration(const CrpClassMember* member, CrpGenerationContext* ctx)
{
	const char* memberName = crpClassMemberGetName(member);
	const char* memberType = crpClassMemberGetType(member);
	int32_t isNative = crpClassMemberIsNative(member);

	if (isNative)
	{
		crpGenerationContextWrite(ctx, "AddNativeMemberVariable(\"%s\", cct::TypeId<%s>());", memberName, memberType);
	}
	else
	{
		const char* nsPath = crpGenerationContextGetNamespacePath(ctx);

		char classVar[512];
		SanitizeIdentifier(memberName, classVar, sizeof(classVar));

		if (nsPath && nsPath[0] != '\0')
		{
			crpGenerationContextWrite(ctx, "const cct::refl::Class* %sClass = cct::refl::GetClassByName(\"%s::%s\"sv);", classVar, nsPath, memberType);
			crpGenerationContextWrite(ctx, "if (%sClass == nullptr)", classVar);
			crpGenerationContextEnterScope(ctx);
			{
				crpGenerationContextWrite(ctx, "%sClass = cct::refl::GetClassByName(\"%s\"sv);", classVar, memberType);
			}
			crpGenerationContextLeaveScope(ctx, NULL);
		}
		else
		{
			crpGenerationContextWrite(ctx, "const cct::refl::Class* %sClass = cct::refl::GetClassByName(\"%s\"sv);", classVar, memberType);
		}
		crpGenerationContextWrite(ctx, "CCT_ASSERT(%sClass != nullptr, \"Could not find class '%s'\");", classVar, memberType);
		crpGenerationContextWrite(ctx, "AddMemberVariable(\"%s\", %sClass);", memberName, classVar);
	}
}

static void OnMethodGeneration(const CrpClassMethod* method, CrpGenerationContext* ctx)
{
	const CrpClass* cls = crpGenerationContextGetClass(ctx);
	const char* className = crpClassGetName(cls);
	const char* methodName = crpClassMethodGetName(method);
	const char* returnType = crpClassMethodGetReturnType(method);
	size_t paramCount = crpClassMethodGetParamCount(method);
	if (!returnType || returnType[0] == '\0')
		returnType = "void";

	// Build call args strings from method params
	char callArgs[2048] = "";
	char callArgsTypes[2048] = "";
	int overflow = 0;

	for (size_t i = 0; i < paramCount; ++i)
	{
		const CrpClassMethodParam* param = crpClassMethodGetParam(method, i);
		const char* paramType = crpClassMethodParamGetType(param);
		const char* paramName = crpClassMethodParamGetName(param);

		if (i > 0)
		{
			if (!AppendToBuffer(callArgs, sizeof(callArgs), ", "))
				overflow = 1;
			if (!AppendToBuffer(callArgsTypes, sizeof(callArgsTypes), ", "))
				overflow = 1;
		}
		if (!AppendToBuffer(callArgs, sizeof(callArgs), paramName ? paramName : ""))
			overflow = 1;
		if (!AppendToBuffer(callArgsTypes, sizeof(callArgsTypes), paramType ? paramType : ""))
			overflow = 1;
	}

	if (overflow)
	{
		fprintf(stderr, "CppPlugin: method signature too long while generating %s::%s\n",
				className ? className : "<null>",
				methodName ? methodName : "<null>");
		return;
	}

	int hasDelegate = crpClassMethodHasDelegate(method);
	int isBoolDelegate = crpClassMethodIsBooleanDelegate(method);
	const char* delegateName = crpClassMethodGetDelegateName(method);

	if (strcmp(returnType, "void") == 0)
	{
		if (hasDelegate && isBoolDelegate)
		{
			crpGenerationContextWrite(ctx, "if (GetCustomDelegate() == nullptr)");
			crpGenerationContextEnterScope(ctx);
			crpGenerationContextWrite(ctx, "CCT_ASSERT_FALSE(\"Missing Delegate\");");
			crpGenerationContextWrite(ctx, "return {\"Invalid delegate pointer\"s};");
			crpGenerationContextLeaveScope(ctx, NULL);
			crpGenerationContextWrite(ctx, "auto func = reinterpret_cast<%s(*)(%s)>(GetCustomDelegate());", returnType, callArgsTypes);
			crpGenerationContextWrite(ctx, "func(%s);", callArgs);
		}
		else if (hasDelegate && delegateName)
		{
			crpGenerationContextWrite(ctx, "%s(%s);", delegateName, callArgs);
		}
		else
		{
			crpGenerationContextWrite(ctx, "static_cast<%s&>(self).%s(%s);", className, methodName, callArgs);
		}
		crpGenerationContextWrite(ctx, "return Any{};");
	}
	else
	{
		if (hasDelegate && isBoolDelegate)
		{
			crpGenerationContextWrite(ctx, "if (GetCustomDelegate() == nullptr)");
			crpGenerationContextEnterScope(ctx);
			crpGenerationContextWrite(ctx, "CCT_ASSERT_FALSE(\"Missing Delegate\");");
			crpGenerationContextWrite(ctx, "return {\"Missing Delegate\"s};");
			crpGenerationContextLeaveScope(ctx, NULL);
			crpGenerationContextWrite(ctx, "auto func = reinterpret_cast<%s(*)(%s)>(GetCustomDelegate());", returnType, callArgsTypes);
			crpGenerationContextWrite(ctx, "return Any::Make<%s>(func(%s));", returnType, callArgs);
		}
		else if (hasDelegate && delegateName)
		{
			crpGenerationContextWrite(ctx, "return Any::Make<%s>(static_cast<%s&>(self).%s(%s));", returnType, className, methodName, callArgs);
		}
		else
		{
			crpGenerationContextWrite(ctx, "auto res = static_cast<%s&>(self).%s(%s);", className, methodName, callArgs);
			crpGenerationContextWrite(ctx, "return cct::Any::Make<%s>(res);", returnType);
		}
	}
}

static void AfterMethodGeneration(const CrpClassMethod* method, CrpGenerationContext* ctx)
{
	const CrpClass* cls = crpGenerationContextGetClass(ctx);
	const char* className = crpClassGetName(cls);
	const char* methodName = crpClassMethodGetName(method);
	const char* returnType = crpClassMethodGetReturnType(method);
	size_t paramCount = crpClassMethodGetParamCount(method);
	if (!returnType || returnType[0] == '\0')
		returnType = "void";

	// Close Invoke() scope
	crpGenerationContextLeaveScope(ctx, NULL);
	// Close class scope
	crpGenerationContextLeaveScope(ctx, ";");

	// Generate delegate function body if needed
	if (crpClassMethodHasDelegate(method))
	{
		char methodParameterPrototype[2048] = "";
		char delegateCallArgs[2048] = "";
		char delegateCallArgsTypes[2048] = "";
		int overflow = 0;

		for (size_t i = 0; i < paramCount; ++i)
		{
			const CrpClassMethodParam* param = crpClassMethodGetParam(method, i);
			const char* paramType = crpClassMethodParamGetType(param);
			const char* paramName = crpClassMethodParamGetName(param);

			if (i > 0)
			{
				if (!AppendToBuffer(methodParameterPrototype, sizeof(methodParameterPrototype), ", "))
					overflow = 1;
				if (!AppendToBuffer(delegateCallArgs, sizeof(delegateCallArgs), ", "))
					overflow = 1;
				if (!AppendToBuffer(delegateCallArgsTypes, sizeof(delegateCallArgsTypes), ", "))
					overflow = 1;
			}

			char paramDecl[512];
			const int written = snprintf(paramDecl, sizeof(paramDecl), "%s %s", paramType ? paramType : "", paramName ? paramName : "");
			if (written < 0 || (size_t)written >= sizeof(paramDecl))
				overflow = 1;
			if (!AppendToBuffer(methodParameterPrototype, sizeof(methodParameterPrototype), paramDecl))
				overflow = 1;
			if (!AppendToBuffer(delegateCallArgs, sizeof(delegateCallArgs), paramName ? paramName : ""))
				overflow = 1;
			if (!AppendToBuffer(delegateCallArgsTypes, sizeof(delegateCallArgsTypes), paramType ? paramType : ""))
				overflow = 1;
		}

		if (overflow)
		{
			fprintf(stderr, "CppPlugin: delegate signature too long while generating %s::%s\n",
					className ? className : "<null>",
					methodName ? methodName : "<null>");
			return;
		}

		crpGenerationContextNewLine(ctx);
		crpGenerationContextWrite(ctx, "%s %s::%s(%s)", returnType, className, methodName, methodParameterPrototype);
		crpGenerationContextEnterScope(ctx);

		const char* delName = crpClassMethodGetDelegateName(method);
		if (delName)
		{
			if (strcmp(returnType, "void") != 0)
				crpGenerationContextWrite(ctx, "return");
			crpGenerationContextWrite(ctx, "%s(%s)", delName, delegateCallArgs);
			crpGenerationContextWrite(ctx, ";");
		}
		else if (crpClassMethodIsBooleanDelegate(method))
		{
			crpGenerationContextWrite(ctx, "const auto* methodClass = GetClass()->GetMethod(\"%s\"sv);", methodName);
			crpGenerationContextWrite(ctx, "if (methodClass == nullptr || methodClass->GetCustomDelegate() == nullptr)");
			crpGenerationContextEnterScope(ctx);
			crpGenerationContextWrite(ctx, "CCT_ASSERT_FALSE(\"Missing Delegate\");");
			crpGenerationContextWrite(ctx, "return");
			if (strcmp(returnType, "void") != 0)
				crpGenerationContextWrite(ctx, "%s{}", returnType);
			crpGenerationContextWrite(ctx, ";");
			crpGenerationContextLeaveScope(ctx, NULL);
			crpGenerationContextWrite(ctx, "auto func = reinterpret_cast<%s(*)(%s)>(methodClass->GetCustomDelegate());", returnType, delegateCallArgsTypes);
			if (strcmp(returnType, "void") != 0)
				crpGenerationContextWrite(ctx, "return");
			crpGenerationContextWrite(ctx, "func(%s)", delegateCallArgs);
			crpGenerationContextWrite(ctx, ";");
		}

		crpGenerationContextLeaveScope(ctx, NULL);
	}

	crpGenerationContextNewLine(ctx);
}

static void AfterClassGeneration(const CrpClass* cls, CrpGenerationContext* ctx)
{
	const char* className = crpClassGetName(cls);

	// Register methods inside Initialize() before closing it
	size_t methodCount = crpClassGetMethodCount(cls);
	for (size_t i = 0; i < methodCount; ++i)
	{
		const CrpClassMethod* method = crpClassGetMethod(cls, i);
		const char* methodName = crpClassMethodGetName(method);
		const char* returnType = crpClassMethodGetReturnType(method);
		size_t paramCount = crpClassMethodGetParamCount(method);
		if (!returnType || returnType[0] == '\0')
			returnType = "void";

		char paramsStr[4096] = "";
		int overflow = 0;
		for (size_t j = 0; j < paramCount; ++j)
		{
			const CrpClassMethodParam* param = crpClassMethodGetParam(method, j);
			const char* paramType = crpClassMethodParamGetType(param);
			char paramEntry[512];
			const int written = snprintf(paramEntry, sizeof(paramEntry), "cct::refl::GetClassByName(\"\"sv, \"%s\"sv)", paramType ? paramType : "");
			if (written < 0 || (size_t)written >= sizeof(paramEntry))
				overflow = 1;
			if (j > 0)
			{
				if (!AppendToBuffer(paramsStr, sizeof(paramsStr), ", "))
					overflow = 1;
			}
			if (!AppendToBuffer(paramsStr, sizeof(paramsStr), paramEntry))
				overflow = 1;
		}

		if (overflow)
		{
			fprintf(stderr, "CppPlugin: parameter metadata too long while generating %s::%s\n",
					className ? className : "<null>",
					methodName ? methodName : "<null>");
			continue;
		}

		crpGenerationContextWrite(ctx, "auto* %sMethod = new %s%sMethod(\"%s\"sv, cct::refl::GetClassByName(\"%s\"sv), { %s }, %zu);",
								  methodName, className, methodName, methodName, returnType, paramsStr, i);
		crpGenerationContextWrite(ctx, "AddMemberFunction(std::unique_ptr<cct::refl::Method>(%sMethod));", methodName);
	}

	// Close Initialize()
	crpGenerationContextLeaveScope(ctx, NULL);

	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "std::unique_ptr<cct::refl::Object> CreateDefaultObject() const override");
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "auto object = std::unique_ptr<cct::refl::Object>(new %s);", className);
		crpGenerationContextWrite(ctx, "if (object)");
		crpGenerationContextEnterScope(ctx);
		{
			crpGenerationContextWrite(ctx, "object->SetDynamicClass(this);");
			crpGenerationContextWrite(ctx, "object->InitializeMemberVariables();");
		}
		crpGenerationContextLeaveScope(ctx, NULL);
		crpGenerationContextWrite(ctx, "return object;");
	}
	crpGenerationContextLeaveScope(ctx, NULL);

	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "cct::refl::Object* GetMemberVariable(std::size_t index, const cct::refl::Object& self) const override");
	crpGenerationContextEnterScope(ctx);
	{
		size_t nonNativeIndex = 0;
		size_t memberCount = crpClassGetMemberCount(cls);
		for (size_t i = 0; i < memberCount; ++i)
		{
			const CrpClassMember* member = crpClassGetMember(cls, i);
			if (crpClassMemberIsNative(member))
				continue;

			const char* memberName = crpClassMemberGetName(member);
			crpGenerationContextWrite(ctx, "if (index == %zu)", nonNativeIndex);
			crpGenerationContextEnterScope(ctx);
			crpGenerationContextWrite(ctx, "return &const_cast<%s&>(static_cast<const %s&>(self)).%s;", className, className, memberName);
			crpGenerationContextLeaveScope(ctx, NULL);
			nonNativeIndex++;
		}
		crpGenerationContextWrite(ctx, "CCT_ASSERT_FALSE(\"Invalid index\");");
		crpGenerationContextWrite(ctx, "return nullptr;");
	}
	crpGenerationContextLeaveScope(ctx, NULL);

	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "void* GetNativeMemberVariable(std::size_t index, const cct::refl::Object& self) const override");
	crpGenerationContextEnterScope(ctx);
	{
		size_t nativeIndex = 0;
		size_t memberCount = crpClassGetMemberCount(cls);
		for (size_t i = 0; i < memberCount; ++i)
		{
			const CrpClassMember* member = crpClassGetMember(cls, i);
			if (!crpClassMemberIsNative(member))
				continue;

			const char* memberName = crpClassMemberGetName(member);
			crpGenerationContextWrite(ctx, "if (index == %zu)", nativeIndex);
			crpGenerationContextEnterScope(ctx);
			crpGenerationContextWrite(ctx, "return &const_cast<%s&>(static_cast<const %s&>(self)).%s;", className, className, memberName);
			crpGenerationContextLeaveScope(ctx, NULL);
			nativeIndex++;
		}
		crpGenerationContextWrite(ctx, "CCT_ASSERT_FALSE(\"Invalid index\");");
		crpGenerationContextWrite(ctx, "return nullptr;");
	}
	crpGenerationContextLeaveScope(ctx, NULL);
	crpGenerationContextLeaveScope(ctx, ";");
	crpGenerationContextNewLine(ctx);
}

static void BeforeGenericClassGeneration(const CrpClass* cls, CrpGenerationContext* ctx)
{
	const char* className = crpClassGetName(cls);
	const char* baseClass = crpClassGetBase(cls);
	const char* nsPath = crpGenerationContextGetNamespacePath(ctx);
	size_t genericFieldCount = crpClassGetGenericTypeParameterFieldCount(cls);

	crpGenerationContextWrite(ctx, "class Internal%sGenericClass : public cct::refl::GenericClass", className);
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "public:");
		crpGenerationContextWrite(ctx, "Internal%sGenericClass() : cct::refl::GenericClass(nullptr, \"%s\"s, nullptr)", className, className);
		crpGenerationContextEnterScope(ctx);
		crpGenerationContextLeaveScope(ctx, NULL);

		crpGenerationContextNewLine(ctx);
		crpGenerationContextWrite(ctx, "void Initialize() override");
		crpGenerationContextEnterScope(ctx);
		{
			if (nsPath && nsPath[0] != '\0')
				crpGenerationContextWrite(ctx, "SetNamespace(GlobalNamespace::Get().GetNamespaceByName(\"%s\"sv));", nsPath);

			if (baseClass && baseClass[0] != '\0')
			{
				crpGenerationContextWrite(ctx, "const Class* baseClass = GetClassByName(\"%s\"sv);", baseClass);
				crpGenerationContextWrite(ctx, "if (baseClass == nullptr)");
				crpGenerationContextEnterScope(ctx);
				{
					if (nsPath && nsPath[0] != '\0')
						crpGenerationContextWrite(ctx, "baseClass = GetClassByName(\"%s::%s\"sv);", nsPath, baseClass);
				}
				crpGenerationContextLeaveScope(ctx, NULL);
				crpGenerationContextWrite(ctx, "CCT_ASSERT(baseClass != nullptr, \"Could not find class '%s'\");", baseClass);
				crpGenerationContextWrite(ctx, "SetBaseClass(baseClass);");
			}

			crpGenerationContextNewLine(ctx);
			crpGenerationContextWrite(ctx, "SetTypeParameterCount(%zu);", genericFieldCount);

			for (size_t i = 0; i < genericFieldCount; ++i)
			{
				const char* fieldName = crpClassGetGenericTypeParameterField(cls, i);
				crpGenerationContextWrite(ctx, "AddTypeParameter(\"%s\");", fieldName);
			}

			crpGenerationContextNewLine(ctx);
			size_t memberCount = crpClassGetMemberCount(cls);
			for (size_t i = 0; i < memberCount; ++i)
			{
				const CrpClassMember* member = crpClassGetMember(cls, i);
				const char* memberName = crpClassMemberGetName(member);
				const char* memberType = crpClassMemberGetType(member);
				int32_t isNative = crpClassMemberIsNative(member);

				if (isNative)
				{
					crpGenerationContextWrite(ctx, "AddNativeMemberVariable(\"%s\", cct::TypeId<%s>());", memberName, memberType);
				}
				else
				{
					crpGenerationContextWrite(ctx, "AddMemberVariable(\"%s\", cct::refl::GetClassByName(\"%s\"sv));", memberName, memberType);
				}
			}
		}
		crpGenerationContextLeaveScope(ctx, NULL);
	}
}

static void AfterGenericClassGeneration(const CrpClass* cls, CrpGenerationContext* ctx)
{
	const char* className = crpClassGetName(cls);
	size_t genericFieldCount = crpClassGetGenericTypeParameterFieldCount(cls);

	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "std::unique_ptr<cct::refl::Object> CreateDefaultObject() const override");
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "CCT_ASSERT_FALSE(\"Cannot instantiate generic class directly. Use CreateDefaultObject(std::span<const Class*>)\");");
		crpGenerationContextWrite(ctx, "return nullptr;");
	}
	crpGenerationContextLeaveScope(ctx, NULL);

	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "std::unique_ptr<cct::refl::Object> CreateDefaultObject(");
	crpGenerationContextWrite(ctx, "    std::span<const cct::refl::Class*> typeArgs) const override");
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "if (typeArgs.size() != %zu)", genericFieldCount);
		crpGenerationContextEnterScope(ctx);
		{
			crpGenerationContextWrite(ctx, "CCT_ASSERT_FALSE(\"Expected %zu type arguments, got %%zu\", typeArgs.size());", genericFieldCount);
			crpGenerationContextWrite(ctx, "return nullptr;");
		}
		crpGenerationContextLeaveScope(ctx, NULL);

		crpGenerationContextWrite(ctx, "auto obj = std::make_unique<%s>();", className);

		for (size_t i = 0; i < genericFieldCount; ++i)
		{
			const char* fieldName = crpClassGetGenericTypeParameterField(cls, i);
			crpGenerationContextWrite(ctx, "obj->%s = typeArgs[%zu];", fieldName, i);
		}

		crpGenerationContextWrite(ctx, "obj->SetDynamicClass(this);");
		crpGenerationContextWrite(ctx, "obj->InitializeMemberVariables();");
		crpGenerationContextWrite(ctx, "return obj;");
	}
	crpGenerationContextLeaveScope(ctx, NULL);

	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "cct::refl::Object* GetMemberVariable(std::size_t, const cct::refl::Object&) const override");
	crpGenerationContextEnterScope(ctx);
	crpGenerationContextWrite(ctx, "return nullptr;");
	crpGenerationContextLeaveScope(ctx, NULL);

	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "void* GetNativeMemberVariable(std::size_t, const cct::refl::Object&) const override");
	crpGenerationContextEnterScope(ctx);
	crpGenerationContextWrite(ctx, "return nullptr;");
	crpGenerationContextLeaveScope(ctx, NULL);
	crpGenerationContextLeaveScope(ctx, ";");
}

static void BeforeTemplateClassGeneration(const CrpClass* cls, CrpGenerationContext* ctx)
{
	const char* className = crpClassGetName(cls);
	size_t specCount = crpClassGetTemplateSpecializationCount(cls);

	crpGenerationContextWrite(ctx, "namespace");
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "class Internal%sTemplateClass : public ::cct::refl::TemplateClass", className);
		crpGenerationContextEnterScope(ctx);
		{
			crpGenerationContextWrite(ctx, "public:");
			crpGenerationContextWrite(ctx, "Internal%sTemplateClass() : ::cct::refl::TemplateClass(nullptr, \"%s\"s, nullptr)", className, className);
			crpGenerationContextEnterScope(ctx);
			crpGenerationContextLeaveScope(ctx, NULL);

			crpGenerationContextNewLine(ctx);
			crpGenerationContextWrite(ctx, "void Initialize() override");
			crpGenerationContextEnterScope(ctx);
			{
				size_t paramCount = crpClassGetTemplateParameterCount(cls);
				for (size_t i = 0; i < paramCount; ++i)
				{
					const CrpTemplateParameter* param = crpClassGetTemplateParameter(cls, i);
					const char* paramName = crpTemplateParameterGetName(param);
					crpGenerationContextWrite(ctx, "cct::refl::TemplateClass::AddTemplateParameter(\"%s\");", paramName);
				}

				if (specCount > 0)
				{
					crpGenerationContextWrite(ctx, "std::vector<std::string> typeArgs;");
				}

				for (size_t i = 0; i < specCount; ++i)
				{
					const char* spec = crpClassGetTemplateSpecialization(cls, i);
					if (spec && spec[0] != '\0')
					{
						char sanitized[512];
						SanitizeSpecialization(spec, sanitized, sizeof(sanitized));

						crpGenerationContextWrite(ctx, "typeArgs.clear();");
						crpGenerationContextWrite(ctx, "typeArgs.push_back(\"%s\");", spec);
						crpGenerationContextWrite(ctx, "RegisterSpecialization(typeArgs, std::make_unique<Internal%s_%s_Class>());", className, sanitized);
					}
				}
			}
			crpGenerationContextLeaveScope(ctx, NULL);
		}
	}
}

static void AfterTemplateClassGeneration(const CrpClass* cls, CrpGenerationContext* ctx)
{
	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "std::unique_ptr<::cct::refl::Object> CreateDefaultObject() const override");
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "CCT_ASSERT_FALSE(\"Cannot instantiate template class directly\");");
		crpGenerationContextWrite(ctx, "return nullptr;");
	}
	crpGenerationContextLeaveScope(ctx, NULL);
	crpGenerationContextLeaveScope(ctx, ";");
	crpGenerationContextNewLine(ctx);
	crpGenerationContextLeaveScope(ctx, NULL);
	crpGenerationContextNewLine(ctx);
}

static void BeforeTemplateSpecializationGeneration(const CrpClass* cls, const char* spec, CrpGenerationContext* ctx)
{
	const char* className = crpClassGetName(cls);
	const char* nsPath = crpGenerationContextGetNamespacePath(ctx);
	char sanitized[512];
	SanitizeSpecialization(spec, sanitized, sizeof(sanitized));

	crpGenerationContextWrite(ctx, "class Internal%s_%s_Class : public cct::refl::Class", className, sanitized);
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "public:");
		crpGenerationContextWrite(ctx, "Internal%s_%s_Class() : cct::refl::Class(nullptr, \"%s<%s>\"s, nullptr) {}", className, sanitized, className, spec);
		crpGenerationContextNewLine(ctx);

		crpGenerationContextWrite(ctx, "void Initialize() override");
		crpGenerationContextEnterScope(ctx);
		{
			if (nsPath && nsPath[0] != '\0')
				crpGenerationContextWrite(ctx, "SetNamespace(GlobalNamespace::Get().GetNamespaceByName(\"%s\"sv));", nsPath);
		}
		crpGenerationContextLeaveScope(ctx, NULL);
		crpGenerationContextNewLine(ctx);
	}
}

static void AfterTemplateSpecializationGeneration(const CrpClass* cls, const char* spec, CrpGenerationContext* ctx)
{
	const char* className = crpClassGetName(cls);

	crpGenerationContextWrite(ctx, "std::unique_ptr<::cct::refl::Object> CreateDefaultObject() const override");
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "auto obj = std::make_unique<%s<%s>>();", className, spec);
		crpGenerationContextWrite(ctx, "obj->SetDynamicClass(this);");
		crpGenerationContextWrite(ctx, "obj->InitializeMemberVariables();");
		crpGenerationContextWrite(ctx, "return obj;");
	}
	crpGenerationContextLeaveScope(ctx, NULL);
	crpGenerationContextNewLine(ctx);

	crpGenerationContextWrite(ctx, "cct::refl::Object* GetMemberVariable(std::size_t, const cct::refl::Object&) const override");
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "return nullptr;");
	}
	crpGenerationContextLeaveScope(ctx, NULL);
	crpGenerationContextNewLine(ctx);

	crpGenerationContextWrite(ctx, "void* GetNativeMemberVariable(std::size_t, const cct::refl::Object&) const override");
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "return nullptr;");
	}
	crpGenerationContextLeaveScope(ctx, NULL);
	crpGenerationContextLeaveScope(ctx, ";");
	crpGenerationContextNewLine(ctx);
}

static void BeforeNamespaceGeneration(const CrpNamespace* ns, CrpGenerationContext* ctx)
{
	const char* nsName = crpNamespaceGetName(ns);
	crpGenerationContextWrite(ctx, "namespace %s", nsName);
	crpGenerationContextEnterScope(ctx);
}

static void AfterNamespaceGeneration(const CrpNamespace* ns, CrpGenerationContext* ctx)
{
	const char* nsName = crpNamespaceGetName(ns);
	char capitalizedNsName[256];
	Capitalize(nsName, capitalizedNsName, sizeof(capitalizedNsName));

	const CrpPackage* package = crpGenerationContextGetPackage(ctx);
	const char* packageName = crpPackageGetName(package);

	size_t nestedCount = crpNamespaceGetNamespaceCount(ns);
	size_t enumCount = crpNamespaceGetEnumCount(ns);
	size_t classCount = crpNamespaceGetClassCount(ns);

	crpGenerationContextWrite(ctx, "class Internal%s_%s_Namespace : public cct::refl::Namespace", packageName, capitalizedNsName);
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "public:");
		crpGenerationContextWrite(ctx, "Internal%s_%s_Namespace() : cct::refl::Namespace(\"%s\"s) {}", packageName, capitalizedNsName, nsName);
		crpGenerationContextNewLine(ctx);

		crpGenerationContextWrite(ctx, "~Internal%s_%s_Namespace() override", packageName, capitalizedNsName);
		crpGenerationContextEnterScope(ctx);
		{
			crpGenerationContextWrite(ctx, "m_classes.clear();");
			crpGenerationContextWrite(ctx, "m_namespaces.clear();");
		}
		crpGenerationContextLeaveScope(ctx, NULL);
		crpGenerationContextNewLine(ctx);

		crpGenerationContextWrite(ctx, "void LoadNamespaces() override");
		crpGenerationContextEnterScope(ctx);
		{
			for (size_t i = 0; i < nestedCount; ++i)
			{
				const CrpNamespace* nested = crpNamespaceGetNamespace(ns, i);
				const char* nestedName = crpNamespaceGetName(nested);
				char capitalizedNestedName[256];
				Capitalize(nestedName, capitalizedNestedName, sizeof(capitalizedNestedName));
				crpGenerationContextWrite(ctx, "AddNamespace(%s::%s::Create%s_%s_NamespaceInstance());",
										  nsName, nestedName, packageName, capitalizedNestedName);
			}
			crpGenerationContextWrite(ctx, "for (auto& ns : m_namespaces)");
			crpGenerationContextEnterScope(ctx);
			{
				crpGenerationContextWrite(ctx, "ns->LoadNamespaces();");
			}
			crpGenerationContextLeaveScope(ctx, NULL);
		}
		crpGenerationContextLeaveScope(ctx, NULL);
		crpGenerationContextNewLine(ctx);

		crpGenerationContextWrite(ctx, "void LoadClasses() override");
		crpGenerationContextEnterScope(ctx);
		{
			crpGenerationContextWrite(ctx, "for(auto& ns : m_namespaces)");
			crpGenerationContextEnterScope(ctx);
			crpGenerationContextWrite(ctx, "ns->LoadClasses();");
			crpGenerationContextLeaveScope(ctx, NULL);

			for (size_t i = 0; i < enumCount; ++i)
			{
				const CrpEnum* enm = crpNamespaceGetEnum(ns, i);
				const char* enumName = crpEnumGetName(enm);
				crpGenerationContextWrite(ctx, "AddClass(std::make_unique<%s::Internal%sEnumerationClass>());", nsName, enumName);
			}

			for (size_t i = 0; i < classCount; ++i)
			{
				const CrpClass* cls = crpNamespaceGetClass(ns, i);
				const char* className = crpClassGetName(cls);
				if (crpClassIsGeneric(cls))
				{
					crpGenerationContextWrite(ctx, "AddClass(std::make_unique<%s::Internal%sGenericClass>());", nsName, className);
				}
				else if (crpClassIsTemplate(cls))
				{
					crpGenerationContextWrite(ctx, "AddClass(std::make_unique<%s::Internal%sTemplateClass>());", nsName, className);

					size_t specCount = crpClassGetTemplateSpecializationCount(cls);
					for (size_t j = 0; j < specCount; ++j)
					{
						const char* spec = crpClassGetTemplateSpecialization(cls, j);
						if (spec && spec[0] != '\0')
						{
							char sanitized[512];
							SanitizeSpecialization(spec, sanitized, sizeof(sanitized));
							crpGenerationContextWrite(ctx, "AddClass(std::make_unique<%s::Internal%s_%s_Class>());", nsName, className, sanitized);
						}
					}
				}
				else
				{
					crpGenerationContextWrite(ctx, "AddClass(std::make_unique<%s::Internal%sClass>());", nsName, className);
				}
			}
		}
		crpGenerationContextLeaveScope(ctx, NULL);
		crpGenerationContextNewLine(ctx);

		crpGenerationContextWrite(ctx, "void InitializeClasses() override");
		crpGenerationContextEnterScope(ctx);
		{
			crpGenerationContextWrite(ctx, "for (auto& ns : m_namespaces)");
			crpGenerationContextEnterScope(ctx);
			{
				crpGenerationContextWrite(ctx, "ns->InitializeClasses();");
			}
			crpGenerationContextLeaveScope(ctx, NULL);
			crpGenerationContextWrite(ctx, "for (auto& klass : m_classes)");
			crpGenerationContextEnterScope(ctx);
			{
				crpGenerationContextWrite(ctx, "klass->Initialize();");
			}
			crpGenerationContextLeaveScope(ctx, NULL);
		}
		crpGenerationContextLeaveScope(ctx, NULL);
	}
	crpGenerationContextLeaveScope(ctx, ";");
	crpGenerationContextNewLine(ctx);

	crpGenerationContextWrite(ctx, "std::unique_ptr<cct::refl::Namespace> Create%s_%s_NamespaceInstance()", packageName, capitalizedNsName);
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "return std::make_unique<Internal%s_%s_Namespace>();", packageName, capitalizedNsName);
	}
	crpGenerationContextLeaveScope(ctx, NULL);
	crpGenerationContextLeaveScope(ctx, NULL);
}

static void BeforePackageGeneration(const CrpPackage* package, CrpGenerationContext* ctx)
{
	const char* packageName = crpPackageGetName(package);

	crpGenerationContextWrite(ctx, "// This file was automatically generated, do not edit");
	crpGenerationContextWrite(ctx, "#include \"%sPackage.gen.hpp\"", packageName);
	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "#include <span>");
	crpGenerationContextWrite(ctx, "#include <string>");
	crpGenerationContextWrite(ctx, "#include <vector>");
	crpGenerationContextNewLine(ctx);
	crpGenerationContextWrite(ctx, "#include <Concerto/Reflection/Enumeration/Enumeration.refl.hpp>");
	crpGenerationContextWrite(ctx, "#include <Concerto/Reflection/Enumeration/EnumerationClass.hpp>");
	crpGenerationContextWrite(ctx, "#include <Concerto/Reflection/EnumValue/EnumValue.hpp>");
	crpGenerationContextWrite(ctx, "#include <Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp>");
	crpGenerationContextWrite(ctx, "#include <Concerto/Reflection/Namespace/Namespace.hpp>");
	crpGenerationContextNewLine(ctx);

	size_t headerCount = crpGenerationContextGetHeaderCount(ctx);
	for (size_t i = 0; i < headerCount; ++i)
	{
		const char* header = crpGenerationContextGetHeader(ctx, i);
		crpGenerationContextWrite(ctx, "#include <%s>", header);
	}
	crpGenerationContextNewLine(ctx);

	crpGenerationContextWrite(ctx, "using namespace std::string_literals;");
	crpGenerationContextWrite(ctx, "using namespace std::string_view_literals;");
	crpGenerationContextWrite(ctx, "using namespace cct::refl;");
	crpGenerationContextNewLine(ctx);
}

static void AfterPackageGeneration(const CrpPackage* package, CrpGenerationContext* ctx)
{
	const char* packageName = crpPackageGetName(package);
	size_t classCount = crpPackageGetClassCount(package);
	size_t nsCount = crpPackageGetNamespaceCount(package);

	crpGenerationContextWrite(ctx, "class Internal%sPackage : public cct::refl::Package", packageName);
	crpGenerationContextEnterScope(ctx);
	{
		crpGenerationContextWrite(ctx, "public:");
		crpGenerationContextWrite(ctx, "Internal%sPackage() : cct::refl::Package(\"%s\"s) {}", packageName, packageName);
		crpGenerationContextNewLine(ctx);

		crpGenerationContextWrite(ctx, "~Internal%sPackage() override", packageName);
		crpGenerationContextEnterScope(ctx);
		{
			for (size_t i = 0; i < classCount; ++i)
			{
				const CrpClass* cls = crpPackageGetClass(package, i);
				const char* className = crpClassGetName(cls);
				crpGenerationContextWrite(ctx, "GlobalNamespace::Get().RemoveClass(\"%s\"sv);", className);
			}

			for (size_t i = 0; i < nsCount; ++i)
			{
				const CrpNamespace* ns = crpPackageGetNamespace(package, i);
				const char* nsName = crpNamespaceGetName(ns);
				crpGenerationContextWrite(ctx, "GlobalNamespace::Get().RemoveNamespace(\"%s\"sv);", nsName);
			}
			crpGenerationContextWrite(ctx, "m_namespaces.clear();");
		}
		crpGenerationContextLeaveScope(ctx, NULL);

		crpGenerationContextNewLine(ctx);
		crpGenerationContextWrite(ctx, "void LoadNamespaces() override");
		crpGenerationContextEnterScope(ctx);
		{
			for (size_t i = 0; i < nsCount; ++i)
			{
				const CrpNamespace* ns = crpPackageGetNamespace(package, i);
				const char* nsName = crpNamespaceGetName(ns);
				char capitalizedNsName[256];
				Capitalize(nsName, capitalizedNsName, sizeof(capitalizedNsName));
				crpGenerationContextWrite(ctx, "AddNamespace(%s::Create%s_%s_NamespaceInstance());", nsName, packageName, capitalizedNsName);
			}
			crpGenerationContextWrite(ctx, "for (auto& ns : m_namespaces)");
			crpGenerationContextEnterScope(ctx);
			{
				crpGenerationContextWrite(ctx, "ns->LoadNamespaces();");
			}
			crpGenerationContextLeaveScope(ctx, NULL);
		}
		crpGenerationContextLeaveScope(ctx, NULL);

		crpGenerationContextNewLine(ctx);
		crpGenerationContextWrite(ctx, "void InitializeNamespaces() override");
		crpGenerationContextEnterScope(ctx);
		{
			for (size_t i = 0; i < classCount; ++i)
			{
				const CrpClass* cls = crpPackageGetClass(package, i);
				const char* className = crpClassGetName(cls);

				if (crpClassIsGeneric(cls))
					crpGenerationContextWrite(ctx, "AddClass(std::make_unique<Internal%sGenericClass>());", className);
				else if (crpClassIsTemplate(cls))
				{
					crpGenerationContextWrite(ctx, "AddClass(std::make_unique<Internal%sTemplateClass>());", className);
					size_t specCount = crpClassGetTemplateSpecializationCount(cls);
					for (size_t j = 0; j < specCount; ++j)
					{
						const char* spec = crpClassGetTemplateSpecialization(cls, j);
						if (spec && spec[0] != '\0')
						{
							char sanitized[512];
							SanitizeSpecialization(spec, sanitized, sizeof(sanitized));
							crpGenerationContextWrite(ctx, "AddClass(std::make_unique<Internal%s_%s_Class>());", className, sanitized);
						}
					}
				}
				else
					crpGenerationContextWrite(ctx, "AddClass(std::make_unique<Internal%sClass>());", className);
			}
			crpGenerationContextNewLine(ctx);
			crpGenerationContextWrite(ctx, "for (auto& ns : m_namespaces)");
			crpGenerationContextEnterScope(ctx);
			crpGenerationContextWrite(ctx, "ns->LoadClasses();");
			crpGenerationContextLeaveScope(ctx, NULL);
		}
		crpGenerationContextLeaveScope(ctx, NULL);

		crpGenerationContextNewLine(ctx);
		crpGenerationContextWrite(ctx, "void InitializeClasses() override");
		crpGenerationContextEnterScope(ctx);
		{
			crpGenerationContextWrite(ctx, "for (auto& ns : m_namespaces)");
			crpGenerationContextEnterScope(ctx);
			{
				crpGenerationContextWrite(ctx, "ns->InitializeClasses();");
			}
			crpGenerationContextLeaveScope(ctx, NULL);
		}
		crpGenerationContextLeaveScope(ctx, NULL);
		crpGenerationContextLeaveScope(ctx, ";");
		crpGenerationContextNewLine(ctx);

		crpGenerationContextWrite(ctx, "std::unique_ptr<cct::refl::Package> Create%sPackage(cct::Logger* logger)", packageName);
		crpGenerationContextEnterScope(ctx);
		crpGenerationContextWrite(ctx, "cct::Logger::SetContext(logger);");
		crpGenerationContextWrite(ctx, "return std::make_unique<Internal%sPackage>();", packageName);
	}
	crpGenerationContextLeaveScope(ctx, NULL);
}

static const CrpPluginFunctionTable g_pluginTable = {
	.apiVersion = CRP_PLUGIN_API_VERSION,
	.GetInfo = GetInfo,
	.GetPriority = NULL,
	.OnLoad = OnLoad,
	.OnUnload = OnUnload,
	.TransformPackage = NULL,

	.BeforePackageGeneration = BeforePackageGeneration,
	.OnPackageGeneration = NULL,
	.AfterPackageGeneration = AfterPackageGeneration,

	.BeforeNamespaceGeneration = BeforeNamespaceGeneration,
	.OnNamespaceGeneration = NULL,
	.AfterNamespaceGeneration = AfterNamespaceGeneration,

	.BeforeClassGeneration = BeforeClassGeneration,
	.OnClassGeneration = NULL,
	.AfterClassGeneration = AfterClassGeneration,

	.BeforeMemberGeneration = NULL,
	.OnMemberGeneration = OnMemberGeneration,
	.AfterMemberGeneration = NULL,

	.BeforeMethodGeneration = BeforeMethodGeneration,
	.OnMethodGeneration = OnMethodGeneration,
	.AfterMethodGeneration = AfterMethodGeneration,

	.BeforeEnumGeneration = BeforeEnumGeneration,
	.OnEnumGeneration = NULL,
	.AfterEnumGeneration = AfterEnumGeneration,

	.BeforeEnumElementGeneration = NULL,
	.OnEnumElementGeneration = OnEnumElementGeneration,
	.AfterEnumElementGeneration = NULL,

	.BeforeTemplateClassGeneration = BeforeTemplateClassGeneration,
	.OnTemplateClassGeneration = NULL,
	.AfterTemplateClassGeneration = AfterTemplateClassGeneration,

	.BeforeGenericClassGeneration = BeforeGenericClassGeneration,
	.OnGenericClassGeneration = NULL,
	.AfterGenericClassGeneration = AfterGenericClassGeneration,

	.BeforeTemplateSpecializationGeneration = BeforeTemplateSpecializationGeneration,
	.OnTemplateSpecializationGeneration = NULL,
	.AfterTemplateSpecializationGeneration = AfterTemplateSpecializationGeneration,

	.HandleAnnotation = NULL,
	.GenerateFile = NULL,
};

// Entry point
CRP_DECLARE_PLUGIN_ENTRY(g_pluginTable)
