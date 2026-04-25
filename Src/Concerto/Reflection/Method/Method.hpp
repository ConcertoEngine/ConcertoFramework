//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_METHOD_HPP
#define CONCERTO_REFLECTION_METHOD_HPP

#include <array>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include <Concerto/Core/Any/Any.hpp>
#include <Concerto/Core/Result/Result.hpp>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	class Class;
	class CCT_REFLECTION_API Method
	{
	public:
		Method(std::string_view name, const Class* returnValue, std::vector<const Class*> parameters, std::size_t index, void* delegate = nullptr);
		virtual ~Method() = default;

		Method(const Method&) = delete;
		Method(Method&&) = default;

		Method& operator=(const Method&) = delete;
		Method& operator=(Method&&) = default;

		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] const Class* GetReturnValue() const;
		[[nodiscard]] std::vector<const Class*> GetParameters() const;
		[[nodiscard]] inline std::size_t GetIndex() const;

		template<typename T, typename... Args>
		Result<T, std::string> Invoke(Object& self, Args&&... args) const;

		inline bool HasAttribute(std::string_view attribute) const;
		inline std::string_view GetAttribute(std::string_view attribute) const;

		// Should be private
		virtual void Initialize() = 0;
		inline void* GetCustomDelegate() const;

	protected:
		void AddAttribute(std::string name, std::string value);
		virtual Result<Any, std::string> Invoke(cct::refl::Object& self, std::span<cct::Any> parameters) const = 0;
		inline void SetCustomDelegate(void* delegate);

	private:
		std::string m_name;
		const Class* m_returnValue;
		std::vector<const Class*> m_parameters;
		std::size_t m_index;
		std::unordered_map<std::string /*name*/, std::string /*value*/> m_attributes;
		void* m_customDelegate;
	};

	template<typename T, typename... Args>
	Result<T, std::string> Method::Invoke(Object& self, Args&&... args) const
	{
		std::array<cct::Any, sizeof...(Args)> erasedArgs = {
			cct::Any::Make<Args>(std::forward<Args>(args))...};

		Result<Any, std::string> result = Invoke(self, erasedArgs);

		if constexpr (std::is_void_v<T>)
		{
			if (result.IsError())
				return std::move(result).GetError();
			return {};
		}
		else
		{
			if (result.IsOk())
				return std::move(result).GetValue().template As<T>();
			return std::move(result).GetError();
		}
	}
} // namespace cct::refl

#include "Concerto/Reflection/Method/Method.inl"

#endif // CONCERTO_REFLECTION_METHOD_HPP