//
// Created by arthur on 31/12/2025
//

#ifndef CONCERTO_REFLECTION_ENUM_VALUE_HPP
#define CONCERTO_REFLECTION_ENUM_VALUE_HPP

#include <string>
#include <string_view>
#include <unordered_map>

#include <Concerto/Core/Types/Types.hpp>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class CCT_REFLECTION_API EnumValue
	{
	public:
		EnumValue(std::string name, cct::Int64 value, std::size_t index);
		~EnumValue() = default;

		EnumValue(const EnumValue&) = delete;
		EnumValue(EnumValue&&) = default;

		EnumValue& operator=(const EnumValue&) = delete;
		EnumValue& operator=(EnumValue&&) = default;

		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] cct::Int64 GetValue() const;
		[[nodiscard]] std::size_t GetIndex() const;

		[[nodiscard]] bool HasAttribute(std::string_view attribute) const;
		[[nodiscard]] std::string_view GetAttribute(std::string_view attribute) const;

	protected:
		void AddAttribute(std::string name, std::string value);

	private:
		std::string m_name;
		cct::Int64 m_value;
		std::size_t m_index;
		std::unordered_map<std::string /*name*/, std::string /*value*/> m_attributes;

		friend class Enumeration;
	};
} // namespace cct::refl

#include "Concerto/Reflection/EnumValue/EnumValue.inl"

#endif // CONCERTO_REFLECTION_ENUM_VALUE_HPP
