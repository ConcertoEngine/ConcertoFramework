//
// Created by arthur on 03/06/2023.
//

#ifndef CONCERTO_CORE_FUNCTIONREF_INL
#define CONCERTO_CORE_FUNCTIONREF_INL

#include <functional>

#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/FunctionRef/FunctionRef.hpp"

namespace cct
{

	template <typename ReturnValue, typename ... Args>
	template <typename Functor> requires (std::is_invocable_r_v<ReturnValue, std::decay_t<Functor>, Args...>)
	FunctionRef<ReturnValue(Args...)>::FunctionRef(Functor&& func) :
		_functionPointer(reinterpret_cast<void*>(std::addressof(func)))
	{
		_callback = [](void* function, Args... args) -> ReturnValue
		{
			return std::invoke(*reinterpret_cast<decltype(std::addressof(func))>(function), std::forward<Args>(args)...);
		};
	}

	template <typename ReturnValue, typename ... Args>
	template <typename Functor> requires (std::is_invocable_r_v<ReturnValue, std::decay_t<Functor>, Args...>)
	FunctionRef<ReturnValue(Args...)>& FunctionRef<ReturnValue(Args...)>::operator=(Functor&& f)
	{
		_functionPointer = reinterpret_cast<void*>(std::addressof(f));
		_callback = [](void* function, Args... args) -> ReturnValue
		{
			return std::invoke(*reinterpret_cast<decltype(std::addressof(f))>(function), std::forward<Args>(args)...);
		};
		return *this;
	}

	template <typename ReturnValue, typename ... Args>
	template <typename ... FunctorArgs> requires(std::is_invocable_r_v<ReturnValue, ReturnValue(Args...), FunctorArgs...>)
	ReturnValue FunctionRef<ReturnValue(Args...)>::operator()(FunctorArgs&&... args) const
	{
		if (this->operator bool() == false)
		{
			CCT_ASSERT_FALSE("ConcertoCore: Invalid function pointer");
			throw std::bad_function_call();
		}
		return _callback(_functionPointer, std::forward<FunctorArgs>(args)...);
	}

	template <typename ReturnValue, typename ... Args>
	FunctionRef<ReturnValue(Args...)>::operator bool() const noexcept
	{
		return _functionPointer != nullptr;
	}
}

#endif // CONCERTO_CORE_FUNCTIONREF_INL
