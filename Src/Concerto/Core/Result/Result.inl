//
// Created by arthur on 09/12/2024.
//

#ifndef CONCERTO_CORE_RESULT_INL
#define CONCERTO_CORE_RESULT_INL

#include <utility>

#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/Result/Result.hpp"

namespace cct
{
	template <typename Value, typename Error>
	requires (!std::is_void_v<Error>)
	constexpr bool Result<Value, Error>::IsError() const
	{
		return m_value.index() == s_errorIndex;
	}

	template <typename Value, typename Error>
		requires (!std::is_void_v<Error>)
	constexpr bool Result<Value, Error>::IsOk() const
	{
		return m_value.index() == s_valueIndex;
	}

	template <typename Value, typename Error>
		requires (!std::is_void_v<Error>)
	constexpr Result<Value, Error>::operator bool() const
	{
		return IsOk();
	}

	template <typename Value, typename Error>
		requires (!std::is_void_v<Error>)
	template <typename ... Args>
	Result<Value, Error>::Result(std::in_place_type_t<Value>, Args&&... args)
	{
		m_value.template emplace<Value>(std::forward<Args>(args)...);
	}

	template <typename Value, typename Error>
		requires (!std::is_void_v<Error>)
	template <typename ... Args>
	Result<Value, Error>::Result(std::in_place_type_t<Error>, Args&&... args)
	{
		m_value.template emplace<Error>(std::forward<Args>(args)...);
	}

	template <typename Value, typename Error>
		requires (!std::is_void_v<Error>)
	constexpr Result<Value, Error>::Result(Value&& value)
	{
		m_value.template emplace<Value>(std::forward<Value>(value));
	}

	template <typename Value, typename Error>
		requires (!std::is_void_v<Error>)
	constexpr Result<Value, Error>::Result(Error&& error)
	{
		m_value.template emplace<Error>(std::forward<Error>(error));
	}

	template <typename Value, typename Error>
		requires (!std::is_void_v<Error>)
	constexpr Value& Result<Value, Error>::GetValue()&
	{
		return std::get<s_valueIndex>(m_value);
	}

	template <typename Value, typename Error>
		requires (!std::is_void_v<Error>)
	constexpr const Value& Result<Value, Error>::GetValue() const&
	{
		return std::get<s_valueIndex>(m_value);
	}

	template <typename Value, typename Error>
		requires (!std::is_void_v<Error>)
	constexpr Value&& Result<Value, Error>::GetValue()&&
	{
		return std::get<s_valueIndex>(std::move(m_value));
	}

	template <typename Value, typename Error>
		requires (!std::is_void_v<Error>)
	constexpr Error& Result<Value, Error>::GetError()&
	{
		return std::get<s_errorIndex>(m_value);
	}

	template <typename Value, typename Error>
		requires (!std::is_void_v<Error>)
	constexpr const Error& Result<Value, Error>::GetError() const&
	{
		return std::get<s_errorIndex>(m_value);
	}

	template <typename Value, typename Error>
		requires (!std::is_void_v<Error>)
	constexpr Error&& Result<Value, Error>::GetError()&&
	{
		return std::get<s_errorIndex>(std::move(m_value));
	}

	/**
	 * Result template specialization
	*/

	template <typename Error>
	requires (!std::is_void_v<Error>)
	template <typename ... Args>
	Result<void, Error>::Result(std::in_place_type_t<Error>, Args&&... args)
	{
		m_value.template emplace<Args...>(std::forward<Args>(args)...);
	}

	template <typename Error>
	requires (!std::is_void_v<Error>)
	constexpr Result<void, Error>::Result(Error&& error) : m_value(std::move(error))
	{
	}

	template <typename Error>
	requires (!std::is_void_v<Error>)
	constexpr Error& Result<void, Error>::GetError() &
	{
		CCT_ASSERT(m_value.has_value(), "Result does not hold an error");
		return *m_value;
	}

	template <typename Error>
	requires (!std::is_void_v<Error>)
	constexpr const Error& Result<void, Error>::GetError() const &
	{
		CCT_ASSERT(m_value.has_value(), "Result does not hold an error");
		return *m_value;
	}

	template <typename Error>
	requires (!std::is_void_v<Error>)
	constexpr Error&& Result<void, Error>::GetError() &&
	{
		CCT_ASSERT(m_value.has_value(), "Result does not hold an error");
		return *std::move(m_value);
	}

	template <typename Error>
	requires (!std::is_void_v<Error>)
	constexpr bool Result<void, Error>::IsError() const
	{
		return m_value.has_value();
	}

	template <typename Error>
	requires (!std::is_void_v<Error>)
	constexpr bool Result<void, Error>::IsOk() const
	{
		return !IsError();
	}

	template <typename Error>
	requires (!std::is_void_v<Error>)
	constexpr Result<void, Error>::operator bool() const
	{
		return IsOk();
	}
}

#endif //CONCERTO_CORE_RESULT_INL
