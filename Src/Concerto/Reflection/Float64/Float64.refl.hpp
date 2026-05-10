#ifndef CONCERTO_REFLECTION_FLOAT64_HPP
#define CONCERTO_REFLECTION_FLOAT64_HPP

#include <Concerto/Core/Types/Types.hpp>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	class CCT_REFL_CLASS() CCT_REFLECTION_API Float64 : public Object
	{
	public:
		Float64() = default;
		explicit Float64(cct::Float64 value);
		~Float64() override = default;

		Float64(const Float64& other);
		Float64(Float64&& other) noexcept;
		Float64& operator=(const Float64& other);
		Float64& operator=(Float64&& other) noexcept;

		void Set(cct::Float64 value);
		[[nodiscard]] cct::Float64 Get() const;

		operator cct::Float64() const;

		Float64& operator=(cct::Float64 value);

		[[nodiscard]] bool operator==(const Float64& other) const;
		[[nodiscard]] bool operator!=(const Float64& other) const;
		[[nodiscard]] bool operator==(cct::Float64 other) const;
		[[nodiscard]] bool operator!=(cct::Float64 other) const;

		[[nodiscard]] std::string ToString() const override;

		CCT_OBJECT(Float64);

	private:
		CCT_NATIVE_MEMBER()
		cct::Float64 m_value = 0.0;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_FLOAT64_HPP
