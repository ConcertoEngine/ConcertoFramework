#ifndef CONCERTO_REFLECTION_UINT64_HPP
#define CONCERTO_REFLECTION_UINT64_HPP

#include <Concerto/Core/Types/Types.hpp>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	class CCT_REFL_CLASS() CCT_REFLECTION_API UInt64 : public Object
	{
	public:
		UInt64() = default;
		explicit UInt64(cct::UInt64 value);
		~UInt64() override = default;

		UInt64(const UInt64& other);
		UInt64(UInt64&& other) noexcept;
		UInt64& operator=(const UInt64& other);
		UInt64& operator=(UInt64&& other) noexcept;

		void Set(cct::UInt64 value);
		[[nodiscard]] cct::UInt64 Get() const;

		operator cct::UInt64() const;

		UInt64& operator=(cct::UInt64 value);

		[[nodiscard]] bool operator==(const UInt64& other) const;
		[[nodiscard]] bool operator!=(const UInt64& other) const;
		[[nodiscard]] bool operator==(cct::UInt64 other) const;
		[[nodiscard]] bool operator!=(cct::UInt64 other) const;

		[[nodiscard]] std::string ToString() const override;

		CCT_OBJECT(UInt64);

	private:
		CCT_NATIVE_MEMBER()
		cct::UInt64 m_value = 0;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_UINT64_HPP
