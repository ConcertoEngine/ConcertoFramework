//
// Created by arthur on 31/12/2025
//

#ifndef CONCERTO_REFLECTION_ENUMERATION_CLASS_HPP
#define CONCERTO_REFLECTION_ENUMERATION_CLASS_HPP

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/Enumeration/EnumIterator.hpp"
#include "Concerto/Reflection/EnumValue/EnumValue.hpp"

namespace cct::refl
{
	class Enumeration;
	class CCT_REFLECTION_API EnumerationClass : public Class
	{
	public:
		EnumerationClass(Namespace* nameSpace, std::string name);
		~EnumerationClass() override = default;

		EnumerationClass(const EnumerationClass&) = delete;
		EnumerationClass(EnumerationClass&&) = default;

		EnumerationClass& operator=(const EnumerationClass&) = delete;
		EnumerationClass& operator=(EnumerationClass&&) = default;

		[[nodiscard]] std::span<const std::unique_ptr<EnumValue>> GetEnumValues() const;
		[[nodiscard]] std::size_t GetEnumValueCount() const;

		[[nodiscard]] const EnumValue* GetEnumValue(std::size_t index) const;
		[[nodiscard]] const EnumValue* GetEnumValue(std::string_view name) const;
		[[nodiscard]] const EnumValue* GetEnumValue(cct::Int64 value) const;

		[[nodiscard]] EnumIterable Enumerate() const;

		[[nodiscard]] std::unique_ptr<Object> CreateDefaultObject() const override;

		template<typename T>
			requires(std::is_base_of_v<cct::refl::Object, T> && std::is_polymorphic_v<T>)
		[[nodiscard]] std::unique_ptr<T> CreateDefaultObject() const
		{
			CCT_ASSERT(T::GetClass() != nullptr, "Class pointer is null");
			if (!T::GetClass()->InheritsFrom(*cct::refl::Object::GetClass()))
			{
				CCT_ASSERT_FALSE("Trying to create object '{}' but it does not inherits from 'Class'", T::GetClass()->GetName());
				return nullptr;
			}
			Object* obj = CreateDefaultObject().release();
			if (!obj)
			{
				CCT_ASSERT_FALSE("Invalid pointer");
				return nullptr;
			}

			return std::unique_ptr<T>(reinterpret_cast<T*>(obj));
		}

	protected:
		void AddEnumValue(std::string name, cct::Int64 value);
		void AddDynamicEnumValue(Enumeration& enumeration, std::string name, cct::Int64 value) const;

	private:
		virtual void Initialize() override;

		std::vector<std::unique_ptr<EnumValue>> m_enumValues;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_ENUMERATION_CLASS_HPP
