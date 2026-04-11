//
// Created by arthur on 21/12/2023.
//

#ifndef CONCERTO_CORE_ASSERT_HPP
#define CONCERTO_CORE_ASSERT_HPP

#include "Concerto/Core/Logger/Logger.hpp"

#if defined(CCT_ENABLE_ASSERTS)
#define CCT_ASSERT(expression, fmt, ...)						\
	do															\
	{															\
		if (!(expression))										\
		{														\
			cct::Logger::Debug(fmt __VA_OPT__(,) __VA_ARGS__);	\
			if (cct::IsDebuggerAttached())						\
			{													\
				CCT_BREAK_IN_DEBUGGER;							\
			}													\
		}														\
	} while (false)
#else
#define CCT_ASSERT(expression, fmt, ...) do { } while (false)
#endif

#define CCT_ASSERT_FALSE(fmt, ...) CCT_ASSERT(false, fmt __VA_OPT__(,) __VA_ARGS__)

#endif //CONCERTO_CORE_ASSERT_HPP
