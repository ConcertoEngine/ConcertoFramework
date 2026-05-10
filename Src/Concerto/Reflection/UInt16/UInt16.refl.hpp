#ifndef CONCERTO_REFLECTION_UINT16_HPP
#define CONCERTO_REFLECTION_UINT16_HPP

#include <Concerto/Core/Types/Types.hpp>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	class CCT_REFL_CLASS() CCT_REFLECTION_API UInt16 : public Object
	{
	public:
		UInt16() = default;
		explicit UInt16(cct::UInt16 value);
		~UInt16() override = default;

		UInt16(const UInt16& other);
		UInt16(UInt16&& other) noexcept;
		UInt16& operator=(const UInt16& other);
		UInt16& operator=(UInt16&& other) noexcept;

		void Set(cct::UInt16 value);
		[[nodiscard]] cct::UInt16 Get() const;

		operator cct::UInt16() const;

		UInt16& operator=(cct::UInt16 value);

		[[nodiscard]] bool operator==(const UInt16& other) const;
		[[nodiscard]] bool operator!=(const UInt16& other) const;
		[[nodiscard]] bool operator==(cct::UInt16 other) const;
		[[nodiscard]] bool operator!=(cct::UInt16 other) const;

		[[nodiscard]] std::string ToString() const override;

		CCT_OBJECT(UInt16);

	private:
		CCT_NATIVE_MEMBER()
		cct::UInt16 m_value = 0;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_UINT16_HPP
