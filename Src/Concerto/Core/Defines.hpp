//
// Created by arthur on 22/02/2023.
//

#ifndef CONCERTO_CORE_DEFINES_HPP
#define CONCERTO_CORE_DEFINES_HPP

#ifdef __cplusplus
#define CCT_LANGUAGE_CPP
#else
#define CCT_LANGUAGE_C
#endif

#ifdef CCT_LANGUAGE_CPP
	#include <cstdint>
	#define CCT_EXTERN_C extern "C"
#else
	#include <stdint.h>
	#define CCT_EXTERN_C
#endif // CCT_LANGUAGE_CPP

#if defined(_WIN32)
	#define CCT_PLATFORM_WINDOWS
#elif defined(__linux__)
	#define CCT_PLATFORM_LINUX
	#define CCT_PLATFORM_POSIX
#elif defined(__FreeBSD__)
	#define CCT_PLATFORM_FREEBSD
	#define CCT_PLATFORM_POSIX
#elif defined(__APPLE__)
	#define CCT_PLATFORM_MACOS
	#define CCT_PLATFORM_POSIX
#elif defined(__EMSCRIPTEN__)
	#define CCT_PLATFORM_WEB
	#define CCT_PLATFORM_POSIX
#endif

//from https://sourceforge.net/p/predef/wiki/Architectures/
#if defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || defined(__arm)
	#define CCT_ARCH_ARM
#elif defined(_M_ARM64) || defined(__aarch64__)
	#define CCT_ARCH_ARM64
#elif defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(__x86_64) || defined(_M_AMD64) || defined(_M_X64)
	#define CCT_ARCH_X86_64
#elif defined(i386) || defined(__i386) || defined(_M_IX86)
	#define CCT_ARCH_X86
#elif defined(__wasm32__)
	#define CCT_ARCH_WASM32
#elif defined(__wasm64__)
	#define CCT_ARCH_WASM64
#endif

#if defined(NDEBUG)
#define CONCERTO_RELEASE
#else
#define CCT_DEBUG
#endif

#ifdef CCT_PLATFORM_WINDOWS
	#define CCT_EXPORT __declspec(dllexport)
	#define CCT_IMPORT __declspec(dllimport)
	#define CCT_NO_INLINE __declspec(noinline)

	#if defined(CCT_ARCH_X86_64)
		#define CCT_CALL __stdcall
	#elif defined(CCT_ARCH_X86)
		#define CCT_CALL __cdecl
	#elif defined(CCT_ARCH_ARM) || defined(CCT_ARCH_ARM64)
    	#define CCT_CALL
	#else
		#error unsupported arch
	#endif
#else
	#define CCT_EXPORT __attribute__((visibility("default")))
	#define CCT_IMPORT __attribute__((visibility("default")))
	#define CCT_CALL
	#define CCT_NO_INLINE __attribute__((noinline))
#endif

#ifdef CCT_CORE_BUILD
	#ifdef CCT_CORE_LIB_STATIC
		#define CCT_CORE_PUBLIC_API
	#else
		#define CCT_CORE_PUBLIC_API CCT_EXPORT
	#endif
#else
	#ifdef CCT_CORE_LIB_STATIC
		#define CCT_CORE_PUBLIC_API
	#else
		#define CCT_CORE_PUBLIC_API CCT_IMPORT
	#endif
#endif

#if defined(CCT_PLATFORM_WINDOWS)
	#define CCT_BREAK_IN_DEBUGGER __debugbreak()
#elif defined(CCT_PLATFORM_POSIX)
	#if defined(CCT_ARCH_ARM64) || defined(CCT_ARCH_ARM)
		#define CCT_BREAK_IN_DEBUGGER __builtin_trap();
	#else
		#define CCT_BREAK_IN_DEBUGGER asm("int $3");
	#endif
#else
	#define CCT_BREAK_IN_DEBUGGER {}
#endif


#ifdef CCT_UNITY_BUILD_ID
#define CCT_ANONYMOUS_NAMESPACE CCT_UNITY_BUILD_ID
#else
#define CCT_ANONYMOUS_NAMESPACE
#endif

#if defined(__clang__)
#define CCT_COMPILER_CLANG
#define CCT_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#define CONCERTO_FILE
#elif defined(__GNUC__)
#define CCT_COMPILER_GCC
#define CCT_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#elif defined(__MINGW32__)
#define CCT_COMPILER_MINGW
#define CCT_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define CCT_COMPILER_MSVC
#define CCT_FUNCTION_SIGNATURE __FUNCSIG__
#else
#pragma message("Unsuported compiler")
#endif

#endif // CONCERTO_CORE_DEFINES_HPP
