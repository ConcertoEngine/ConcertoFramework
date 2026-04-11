//
// Created by arthur on 01/08/2022.
//

#ifndef CONCERTO_CORE_DEFEREDEXIT_INL
#define CONCERTO_CORE_DEFEREDEXIT_INL

namespace cct
{
	template <typename F>
	DeferredExit<F>::DeferredExit(F&& functor) : m_functor(std::move(functor))
	{
	}

	template <typename F>
	DeferredExit<F>::~DeferredExit()
	{
		m_functor();
	}
}
#endif //CONCERTO_CORE_DEFEREDEXIT_INL