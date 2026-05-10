#ifndef CONCERTO_REFLECTION_INT8_HPP
#define CONCERTO_REFLECTION_INT8_HPP

#include <Concerto/Core/Types/Types.hpp>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	class CCT_REFL_CLASS() CCT_REFLECTION_API Int8 : public Object
	{
	public:
		Int8() = default;
		explicit Int8(cct::Int8 value);
		~Int8() override = default;

		Int8(const Int8& other);
		Int8(Int8&& other) noexcept;
		Int8& operator=(const Int8& other);
		Int8& operator=(Int8&& other) noexcept;

		void Set(cct::Int8 value);
		[[nodiscard]] cct::Int8 Get() const;

		operator cct::Int8() const;

		Int8& operator=(cct::Int8 value);

		[[nodiscard]] bool operator==(const Int8& other) const;
		[[nodiscard]] bool operator!=(const Int8& other) const;
		[[nodiscard]] bool operator==(cct::Int8 other) const;
		[[nodiscard]] bool operator!=(cct::Int8 other) const;

		[[nodiscard]] std::string ToString() const override;

		CCT_OBJECT(Int8);

	private:
		CCT_NATIVE_MEMBER()
		cct::Int8 m_value = 0;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_INT8_HPP
