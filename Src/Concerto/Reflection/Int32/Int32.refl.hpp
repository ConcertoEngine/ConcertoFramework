#ifndef CONCERTO_REFLECTION_INT32_HPP
#define CONCERTO_REFLECTION_INT32_HPP

#include <Concerto/Core/Types/Types.hpp>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	class CCT_REFL_CLASS() CCT_REFLECTION_API Int32 : public Object
	{
	public:
		Int32() = default;
		explicit Int32(cct::Int32 value);
		~Int32() override = default;

		Int32(const Int32& other);
		Int32(Int32&& other) noexcept;
		Int32& operator=(const Int32& other);
		Int32& operator=(Int32&& other) noexcept;

		void Set(cct::Int32 value);
		[[nodiscard]] cct::Int32 Get() const;

		operator cct::Int32() const;

		Int32& operator=(cct::Int32 value);

		[[nodiscard]] bool operator==(const Int32& other) const;
		[[nodiscard]] bool operator!=(const Int32& other) const;
		[[nodiscard]] bool operator==(cct::Int32 other) const;
		[[nodiscard]] bool operator!=(cct::Int32 other) const;

		CCT_OBJECT(Int32);

	private:
		CCT_NATIVE_MEMBER()
		cct::Int32 m_value = 0;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_INT32_HPP
