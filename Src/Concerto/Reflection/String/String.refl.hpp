//
// Created by arthur on 27/02/2026.
//

#ifndef CONCERTO_REFLECTION_STRING_HPP
#define CONCERTO_REFLECTION_STRING_HPP

#include <string>
#include <string_view>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	/// A reflectable string type that emits OnValueChanged whenever its value changes.
	class CCT_REFL_CLASS() CCT_REFLECTION_API String : public Object
	{
	public:
		String() = default;
		explicit String(std::string_view value);
		explicit String(std::string value);
		~String() override = default;

		String(const String& other);
		String(String&& other) noexcept;
		String& operator=(const String& other);
		String& operator=(String&& other) noexcept;

		void Set(std::string_view value);
		void Set(std::string value);

		[[nodiscard]] const std::string& Get() const;
		[[nodiscard]] std::string& Get();

		operator std::string_view() const;

		String& operator=(std::string_view value);
		String& operator=(std::string value);

		[[nodiscard]] bool operator==(const String& other) const;
		[[nodiscard]] bool operator!=(const String& other) const;
		[[nodiscard]] bool operator==(std::string_view other) const;
		[[nodiscard]] bool operator!=(std::string_view other) const;

		CCT_OBJECT(String);

	private:
		CCT_NATIVE_MEMBER()
		std::string m_value;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_STRING_HPP
