#ifndef CONCERTO_REFLECTION_FLOAT32_HPP
#define CONCERTO_REFLECTION_FLOAT32_HPP

#include <Concerto/Core/Types/Types.hpp>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	class CCT_REFL_CLASS() CCT_REFLECTION_API Float32 : public Object
	{
	public:
		Float32() = default;
		explicit Float32(cct::Float32 value);
		~Float32() override = default;

		Float32(const Float32& other);
		Float32(Float32&& other) noexcept;
		Float32& operator=(const Float32& other);
		Float32& operator=(Float32&& other) noexcept;

		void Set(cct::Float32 value);
		[[nodiscard]] cct::Float32 Get() const;

		operator cct::Float32() const;

		Float32& operator=(cct::Float32 value);

		[[nodiscard]] bool operator==(const Float32& other) const;
		[[nodiscard]] bool operator!=(const Float32& other) const;
		[[nodiscard]] bool operator==(cct::Float32 other) const;
		[[nodiscard]] bool operator!=(cct::Float32 other) const;

		CCT_OBJECT(Float32);

	private:
		CCT_NATIVE_MEMBER()
		cct::Float32 m_value = 0.0F;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_FLOAT32_HPP
