//
// Created by arthur on 31/12/2025
//

#ifndef CONCERTO_REFLECTION_ENUMERATION_HPP
#define CONCERTO_REFLECTION_ENUMERATION_HPP

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <Concerto/Core/Types/Types.hpp>

#include "Concerto/Reflection/Enumeration/EnumIterator.hpp"
#include "Concerto/Reflection/EnumValue/EnumValue.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	class CCT_REFL_CLASS() CCT_REFLECTION_API Enumeration : public cct::refl::Object
	{
	public:
		Enumeration();
		~Enumeration() override = default;

		Enumeration(const Enumeration&) = delete;
		Enumeration(Enumeration&&) = default;

		Enumeration& operator=(const Enumeration&) = delete;
		Enumeration& operator=(Enumeration&&) = default;

		[[nodiscard]] std::span<const std::unique_ptr<EnumValue>> GetEnumValues() const;
		[[nodiscard]] std::size_t GetEnumValueCount() const;

		[[nodiscard]] const EnumValue* GetEnumValue(std::size_t index) const;
		[[nodiscard]] const EnumValue* GetEnumValue(std::string_view name) const;
		[[nodiscard]] const EnumValue* GetEnumValue(cct::Int64 value) const;

		[[nodiscard]] cct::Int64 GetValue() const;
		void SetEnumValue(cct::Int64 value);

		[[nodiscard]] EnumIterable Enumerate() const;

		[[nodiscard]] std::string ToString(cct::Int64 value) const;
		[[nodiscard]] std::string ToString() const;

		[[nodiscard]] cct::Int64 FromString(std::string_view name) const;

		[[nodiscard]] bool HasFlag(cct::Int64 value, cct::Int64 flag) const;
		cct::Int64 SetFlag(cct::Int64 value, cct::Int64 flag) const;
		cct::Int64 ToggleFlag(cct::Int64 value, cct::Int64 flag) const;

		CCT_OBJECT(Enumeration);

	protected:
		void AddEnumValue(std::string name, cct::Int64 value);

	private:
		std::vector<std::unique_ptr<EnumValue>> m_enumValues;
		cct::Int64 m_value;

		friend class EnumerationClass;
	};
} // namespace cct::refl

#include "Concerto/Reflection/Enumeration/Enumeration.inl"

#endif // CONCERTO_REFLECTION_ENUMERATION_HPP
