//
// Created by arthur on 24/08/2023.
//

#ifndef CONCERTO_PROFILER_PROFILER_HPP
#define CONCERTO_PROFILER_PROFILER_HPP

#ifdef CCT_PROFILING
#include <source_location>

#include <tracy/Tracy.hpp>
#define CCT_PROFILER_SCOPE(name) ZoneScopedN(name)
#define CCT_AUTO_PROFILER_SCOPE() ZoneScoped
#define CCT_FRAME_MARK() FrameMark
#else
#define CCT_PROFILER_SCOPE(name)
#define CCT_AUTO_PROFILER_SCOPE()
#define CCT_FRAME_MARK()
#endif

#endif // CONCERTO_PROFILER_PROFILER_HPP
