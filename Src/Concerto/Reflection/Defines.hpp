//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_DEFINE_HPP
#define CONCERTO_REFLECTION_DEFINE_HPP

#include <Concerto/Core/Assert.hpp>
#include <Concerto/Core/Types/Types.hpp>

#ifdef CCT_REFLECTION_STATIC
#define CCT_REFLECTION_API
#else
#ifdef CCT_REFLECTION_BUILD
#define CCT_REFLECTION_API CCT_EXPORT
#else
#define CCT_REFLECTION_API CCT_IMPORT
#endif // CCT_REFLECTION_BUILD
#endif // CCT_REFLECTION_STATIC

#if defined(CCT_REFLECTION_PKG_GENERATOR_BUILD)

#define CCT_PACKAGE(...) [[clang::annotate("cct::Package" __VA_OPT__(, ) __VA_ARGS__)]]
#define CCT_CLASS(...) [[clang::annotate("cct::Class" __VA_OPT__(, ) __VA_ARGS__)]]
#define CCT_GENERIC_CLASS(...) [[clang::annotate("cct::GenericClass" __VA_OPT__(, ) __VA_ARGS__)]]
#define CCT_GENERIC_TYPE() [[clang::annotate("cct::GenericType")]]
#define CCT_MEMBER(...) [[clang::annotate("cct::Member" __VA_OPT__(, ) __VA_ARGS__)]]
#define CCT_NATIVE_MEMBER(...) [[clang::annotate("cct::NativeMember" __VA_OPT__(, ) __VA_ARGS__)]]
#define CCT_METHOD(...) [[clang::annotate("cct::Method" __VA_OPT__(, ) __VA_ARGS__)]]
#define CCT_ENUM(...) [[clang::annotate("cct::Enum" __VA_OPT__(, ) __VA_ARGS__)]]
#define CCT_ENUM_VALUE(...) [[clang::annotate("cct::EnumValue" __VA_OPT__(, ) __VA_ARGS__)]]

#else

#define CCT_PACKAGE(...)
#define CCT_CLASS(...)
#define CCT_GENERIC_CLASS(...)
#define CCT_GENERIC_TYPE()
#define CCT_MEMBER(...)
#define CCT_NATIVE_MEMBER(...)
#define CCT_METHOD(...)
#define CCT_ENUM(...)
#define CCT_ENUM_VALUE(...)

#endif // CCT_REFLECTION_PKG_GENERATOR_BUILD

#if defined(CCT_REFLECTION_BUILD) && defined(CCT_REFLECTION_PKG_GENERATOR_BUILD)

#define CCT_REFL_PACKAGE(...) CCT_PACKAGE(__VA_ARGS__)
#define CCT_REFL_CLASS(...) CCT_CLASS(__VA_ARGS__)
#define CCT_REFL_MEMBER(...) CCT_MEMBER(__VA_ARGS__)
#define CCT_REFL_METHOD(...) CCT_METHOD(__VA_ARGS__)
#define CCT_REFL_ENUM(...) CCT_ENUM(__VA_ARGS__)
#define CCT_REFL_ENUM_VALUE(...) CCT_ENUM_VALUE(__VA_ARGS__)

#else

#define CCT_REFL_PACKAGE(...)
#define CCT_REFL_CLASS(...)
#define CCT_REFL_MEMBER(...)
#define CCT_REFL_METHOD(...)
#define CCT_REFL_ENUM(...)
#define CCT_REFL_ENUM_VALUE(...)

#endif

#include <Concerto/Core/Logger/LogMacros.hpp>

#define CCT_REFL_LOG_TRACE(channel, fmt, ...) CCT_LOG_TRACE("Reflection", channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_REFL_LOG_DEBUG(channel, fmt, ...) CCT_LOG_DEBUG("Reflection", channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_REFL_LOG_INFO(channel, fmt, ...) CCT_LOG_INFO("Reflection", channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_REFL_LOG_WARN(channel, fmt, ...) CCT_LOG_WARN("Reflection", channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_REFL_LOG_ERROR(channel, fmt, ...) CCT_LOG_ERROR("Reflection", channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_REFL_LOG_CRITICAL(channel, fmt, ...) CCT_LOG_CRITICAL("Reflection", channel, fmt __VA_OPT__(, ) __VA_ARGS__)


#endif // CONCERTO_REFLECTION_DEFINE_HPP