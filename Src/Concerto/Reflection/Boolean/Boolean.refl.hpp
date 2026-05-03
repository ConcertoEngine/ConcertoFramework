#ifndef CONCERTO_REFLECTION_BOOLEAN_HPP
#define CONCERTO_REFLECTION_BOOLEAN_HPP

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	/// A reflectable boolean type that emits OnValueChanged whenever its value changes.
	class CCT_REFL_CLASS() CCT_REFLECTION_API Boolean : public Object
	{
	public:
		Boolean() = default;
		explicit Boolean(bool value);
		~Boolean() override = default;

		Boolean(const Boolean& other);
		Boolean(Boolean&& other) noexcept;
		Boolean& operator=(const Boolean& other);
		Boolean& operator=(Boolean&& other) noexcept;

		void Set(bool value);
		[[nodiscard]] bool Get() const;

		operator bool() const;

		Boolean& operator=(bool value);

		[[nodiscard]] bool operator==(const Boolean& other) const;
		[[nodiscard]] bool operator!=(const Boolean& other) const;
		[[nodiscard]] bool operator==(bool other) const;
		[[nodiscard]] bool operator!=(bool other) const;

		CCT_OBJECT(Boolean);

	private:
		CCT_NATIVE_MEMBER()
		bool m_value = false;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_BOOLEAN_HPP
