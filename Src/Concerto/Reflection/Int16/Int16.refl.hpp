#ifndef CONCERTO_REFLECTION_INT16_HPP
#define CONCERTO_REFLECTION_INT16_HPP

#include <Concerto/Core/Types/Types.hpp>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	class CCT_REFL_CLASS() CCT_REFLECTION_API Int16 : public Object
	{
	public:
		Int16() = default;
		explicit Int16(cct::Int16 value);
		~Int16() override = default;

		Int16(const Int16& other);
		Int16(Int16&& other) noexcept;
		Int16& operator=(const Int16& other);
		Int16& operator=(Int16&& other) noexcept;

		void Set(cct::Int16 value);
		[[nodiscard]] cct::Int16 Get() const;

		operator cct::Int16() const;

		Int16& operator=(cct::Int16 value);

		[[nodiscard]] bool operator==(const Int16& other) const;
		[[nodiscard]] bool operator!=(const Int16& other) const;
		[[nodiscard]] bool operator==(cct::Int16 other) const;
		[[nodiscard]] bool operator!=(cct::Int16 other) const;

		[[nodiscard]] std::string ToString() const override;

		CCT_OBJECT(Int16);

	private:
		CCT_NATIVE_MEMBER()
		cct::Int16 m_value = 0;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_INT16_HPP
