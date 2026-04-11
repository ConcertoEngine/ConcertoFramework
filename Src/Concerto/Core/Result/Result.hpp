//
// Created by arthur on 09/12/2024.
//

#ifndef CONCERTO_CORE_RESULT_HPP
#define CONCERTO_CORE_RESULT_HPP

#include <variant>
#include <type_traits>
#include <optional>

namespace cct
{
	template<typename Value, typename Error>
	requires (!std::is_void_v<Error>)
	class Result
	{
	public:
		constexpr Result() requires(std::default_initializable<Value>) = default;

		template<typename... Args>
		Result(std::in_place_type_t<Value>, Args&&... args);

		template<typename... Args>
		Result(std::in_place_type_t<Error>, Args&&... args);

		constexpr Result(Value&& value);
		constexpr Result(Error&& error);

		constexpr Value& GetValue() &;
		constexpr const Value& GetValue() const &;
		constexpr Value&& GetValue() &&;

		constexpr Error& GetError() &;
		constexpr const Error& GetError() const &;
		constexpr Error&& GetError() &&;

		constexpr bool IsError() const;
		constexpr bool IsOk() const;

		constexpr operator bool() const;

	private:
		std::variant<Value, Error> m_value;
		static constexpr decltype(m_value.index()) s_valueIndex = 0;
		static constexpr decltype(m_value.index()) s_errorIndex = 1;
	};

	template<typename Error>
	requires (!std::is_void_v<Error>)
	class Result<void, Error>
	{
	public:
		Result() = default;
		template<typename... Args>
		Result(std::in_place_type_t<Error>, Args&&... args);

		constexpr Result(Error&& error);

		constexpr Error& GetError()&;
		constexpr const Error& GetError() const&;
		constexpr Error&& GetError()&&;

		constexpr bool IsError() const;
		constexpr bool IsOk() const;

		constexpr operator bool() const;

	private:
		std::optional<Error> m_value;
	};
}

#include "Concerto/Core/Result/Result.inl"
#endif //CONCERTO_CORE_RESULT_HPP