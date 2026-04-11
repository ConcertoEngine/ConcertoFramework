//
// Created by arthur on 02/06/2023.
//

#ifndef CONCERTO_CORE_DYNLIB_INL
#define CONCERTO_CORE_DYNLIB_INL

#include <stdexcept>

#include "Concerto/Core/DynLib/DynLib.hpp"

namespace cct
{
	template <typename ReturnValue, typename ... Args>
	ReturnValue DynLib::Invoke(const std::string& functionName, Args&&... args)
	{
		auto func = GetFunction<ReturnValue, Args...>(functionName);
		if (!func)
		{
			CCT_ASSERT_FALSE("Invalid function pointer");
			throw std::runtime_error("Invalid function pointer");
		}
		return func(std::forward<Args>(args)...);
	}

	template <typename ReturnValue, typename ... Args>
	FunctionRef<ReturnValue(Args...)> DynLib::GetFunction(const std::string& functionName)
	{
		void* symbol = GetSymbol(functionName);
		if (symbol == nullptr)
		{
			CCT_ASSERT_FALSE("ConcertoCore: Invalid symbol pointer '{}'", functionName);
			return FunctionRef<ReturnValue(Args...)>();
		}
		using Func = ReturnValue(*)(Args...);
		return FunctionRef<ReturnValue(Args...)>(*reinterpret_cast<Func>(symbol));
	}

	template <typename T>
	T* DynLib::GetValue(const std::string& valueName)
	{
		void* symbol = GetSymbol(valueName);
		if (symbol == nullptr)
		{
			CCT_ASSERT_FALSE("ConcertoCore: Invalid symbol pointer '{}'", valueName);
			return nullptr;
		}
		return static_cast<T*>(symbol);
	}
}// namespace cct

#endif//CONCERTO_CORE_DYNLIB_INL