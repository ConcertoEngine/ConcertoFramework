#ifndef CONCERTO_REFLECTION_UINT32_HPP
#define CONCERTO_REFLECTION_UINT32_HPP

#include <Concerto/Core/Types/Types.hpp>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	class CCT_REFL_CLASS() CCT_REFLECTION_API UInt32 : public Object
	{
	public:
		UInt32() = default;
		explicit UInt32(cct::UInt32 value);
		~UInt32() override = default;

		UInt32(const UInt32& other);
		UInt32(UInt32&& other) noexcept;
		UInt32& operator=(const UInt32& other);
		UInt32& operator=(UInt32&& other) noexcept;

		void Set(cct::UInt32 value);
		[[nodiscard]] cct::UInt32 Get() const;

		operator cct::UInt32() const;

		UInt32& operator=(cct::UInt32 value);

		[[nodiscard]] bool operator==(const UInt32& other) const;
		[[nodiscard]] bool operator!=(const UInt32& other) const;
		[[nodiscard]] bool operator==(cct::UInt32 other) const;
		[[nodiscard]] bool operator!=(cct::UInt32 other) const;

		CCT_OBJECT(UInt32);

	private:
		CCT_NATIVE_MEMBER()
		cct::UInt32 m_value = 0;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_UINT32_HPP
