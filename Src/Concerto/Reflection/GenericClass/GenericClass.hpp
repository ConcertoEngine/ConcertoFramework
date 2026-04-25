//
// Created by arthur on 16/12/2025
//

#ifndef CONCERTO_REFLECTION_GENERIC_CLASS_HPP
#define CONCERTO_REFLECTION_GENERIC_CLASS_HPP

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "Concerto/Reflection/Class/Class.hpp"

namespace cct::refl
{
	class CCT_REFLECTION_API GenericClass : public Class
	{
	public:
		GenericClass(Namespace* nameSpace, std::string name, const Class* baseClass);
		~GenericClass() override = default;

		GenericClass(const GenericClass&) = delete;
		GenericClass(GenericClass&&) = default;

		GenericClass& operator=(const GenericClass&) = delete;
		GenericClass& operator=(GenericClass&&) = default;

		[[nodiscard]] std::size_t GetTypeParameterCount() const;

		[[nodiscard]] std::span<const std::string> GetTypeParameterNames() const;

		[[nodiscard]] std::unique_ptr<Object> CreateDefaultObject() const override;

		[[nodiscard]] virtual std::unique_ptr<Object> CreateDefaultObject(std::span<const Class*> typeArgs) const;

		[[nodiscard]] bool IsGenericClass() const override;

	protected:
		void AddTypeParameter(std::string fieldName);

		void SetTypeParameterCount(std::size_t count);

	private:
		virtual void Initialize() override;

		std::vector<std::string> m_typeParameterNames;
		std::size_t m_typeParameterCount;
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_GENERIC_CLASS_HPP
