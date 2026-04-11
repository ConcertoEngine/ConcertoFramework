//
// Created by arthur on 24/08/2023.
//
#ifndef CONCERTO_GRAPHICS_PROFILER_PROFILER_HPP
#define CONCERTO_GRAPHICS_PROFILER_PROFILER_HPP


#ifdef CCT_GFX_PROFILING
#include <tracy/Tracy.hpp>
#include <source_location>
#define CCT_GFX_PROFILER_SCOPE(name) ZoneScopedN(name)
#define CCT_GFX_AUTO_PROFILER_SCOPE() ZoneScoped
#define CCT_GFX_FRAME_MARK FrameMark
#else
#define CCT_GFX_PROFILER_SCOPE(name)
#define CCT_GFX_AUTO_PROFILER_SCOPE()
#define CCT_GFX_FRAME_MARK
#endif


#endif //CONCERTO_GRAPHICS_PROFILER_PROFILER_HPP