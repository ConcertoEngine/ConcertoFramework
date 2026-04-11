//
// Created by arthur on 01/08/2022.
//

#ifndef CONCERTO_CORE_DEFEREDEXIT_HPP
#define CONCERTO_CORE_DEFEREDEXIT_HPP

#include <algorithm>

namespace cct
{
	template<typename F>
	class DeferredExit final
	{
	public:
		DeferredExit() = delete;
		DeferredExit(F&& functor);
		DeferredExit(DeferredExit&&) = default;
		DeferredExit(const DeferredExit&) = delete;

		~DeferredExit();

		DeferredExit& operator=(DeferredExit&&) = default;
		DeferredExit& operator=(const DeferredExit&) = delete;
	private:
		F m_functor;
	};
}

#include "Concerto/Core/DeferredExit/DeferredExit.inl"

#endif //CONCERTO_CORE_DEFEREDEXIT_HPP