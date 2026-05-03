#ifndef CONCERTO_REFLECTION_UINT8_HPP
#define CONCERTO_REFLECTION_UINT8_HPP

#include <Concerto/Core/Types/Types.hpp>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	class CCT_REFL_CLASS() CCT_REFLECTION_API UInt8 : public Object
	{
	public:
		UInt8() = default;
		explicit UInt8(cct::UInt8 value);
		~UInt8() override = default;

		UInt8(const UInt8& other);
		UInt8(UInt8&& other) noexcept;
		UInt8& operator=(const UInt8& other);
		UInt8& operator=(UInt8&& other) noexcept;

		void Set(cct::UInt8 value);
		[[nodiscard]] cct::UInt8 Get() const;

		operator cct::UInt8() const;

		UInt8& operator=(cct::UInt8 value);

		[[nodiscard]] bool operator==(const UInt8& other) const;
		[[nodiscard]] bool operator!=(const UInt8& other) const;
		[[nodiscard]] bool operator==(cct::UInt8 other) const;
		[[nodiscard]] bool operator!=(cct::UInt8 other) const;

		CCT_OBJECT(UInt8);

	private:
		CCT_NATIVE_MEMBER()
		cct::UInt8 m_value = 0;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_UINT8_HPP
