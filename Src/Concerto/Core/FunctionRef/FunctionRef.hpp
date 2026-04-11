//
// Created by arthur on 03/06/2023.
//

#ifndef CONCERTO_CORE_FUNCTIONREF_HPP
#define CONCERTO_CORE_FUNCTIONREF_HPP

#include <type_traits>

namespace cct
{
	template<typename T>
	class FunctionRef;

	template<typename ReturnValue, typename... Args>
	class FunctionRef<ReturnValue(Args...)> final
	{
	public:
		using Signature = ReturnValue(Args...);
		FunctionRef() = default;
		template<typename Functor>
			requires(std::is_invocable_r_v<ReturnValue, std::decay_t<Functor>, Args...>)
		FunctionRef(Functor&& func);
		FunctionRef(const FunctionRef&) = default;
		FunctionRef(FunctionRef&&) = default;
		~FunctionRef() = default;

		FunctionRef& operator=(const FunctionRef&) = default;
		FunctionRef& operator=(FunctionRef&&) = default;

		explicit operator bool() const noexcept;

		template<typename Functor>
			requires(std::is_invocable_r_v<ReturnValue, std::decay_t<Functor>, Args...>)
		FunctionRef& operator=(Functor&& f);

		template<typename... FunctorArgs>
			requires(std::is_invocable_r_v<ReturnValue, ReturnValue(Args...), FunctorArgs...>)
		ReturnValue operator()(FunctorArgs&&... args) const;

	private:
		using CallBack = ReturnValue(*)(void*, Args...);
		CallBack _callback;
		void* _functionPointer = nullptr;
	};

	template<typename ReturnValue, typename... Args>
	FunctionRef(ReturnValue(*)(Args...)) -> FunctionRef<ReturnValue(Args...)>;
}

#include "Concerto/Core/FunctionRef/FunctionRef.inl"

#endif // CONCERTO_CORE_FUNCTIONREF_HPP
