#ifndef CONCERTO_REFLECTION_INT64_HPP
#define CONCERTO_REFLECTION_INT64_HPP

#include <Concerto/Core/Types/Types.hpp>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	class CCT_REFL_CLASS() CCT_REFLECTION_API Int64 : public Object
	{
	public:
		Int64() = default;
		explicit Int64(cct::Int64 value);
		~Int64() override = default;

		Int64(const Int64& other);
		Int64(Int64&& other) noexcept;
		Int64& operator=(const Int64& other);
		Int64& operator=(Int64&& other) noexcept;

		void Set(cct::Int64 value);
		[[nodiscard]] cct::Int64 Get() const;

		operator cct::Int64() const;

		Int64& operator=(cct::Int64 value);

		[[nodiscard]] bool operator==(const Int64& other) const;
		[[nodiscard]] bool operator!=(const Int64& other) const;
		[[nodiscard]] bool operator==(cct::Int64 other) const;
		[[nodiscard]] bool operator!=(cct::Int64 other) const;

		[[nodiscard]] std::string ToString() const override;

		CCT_OBJECT(Int64);

	private:
		CCT_NATIVE_MEMBER()
		cct::Int64 m_value = 0;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_INT64_HPP
