//
// Created by arthur on 10/12/2024.
//

#ifndef CONCERTO_REFLECTION_TEMPLATE_CLASS_HPP
#define CONCERTO_REFLECTION_TEMPLATE_CLASS_HPP

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Concerto/Reflection/Class/Class.hpp"

namespace cct::refl
{
	class CCT_REFLECTION_API TemplateClass : public Class
	{
	public:
		TemplateClass(Namespace* nameSpace, std::string name, const Class* baseClass);
		~TemplateClass() override = default;

		TemplateClass(const TemplateClass&) = delete;
		TemplateClass(TemplateClass&&) = default;

		TemplateClass& operator=(const TemplateClass&) = delete;
		TemplateClass& operator=(TemplateClass&&) = default;

		[[nodiscard]] std::span<const std::string> GetTemplateParameters() const;
		[[nodiscard]] const Class* GetSpecialization(std::span<std::string_view> typeArgs) const;
		[[nodiscard]] const Class* GetSpecialization(std::string_view typeArg) const;
		[[nodiscard]] bool HasSpecialization(std::span<std::string_view> typeArgs) const;

		[[nodiscard]] bool IsTemplateClass() const override;

		virtual std::unique_ptr<cct::refl::Object> CreateDefaultObject() const override;

	protected:
		void AddTemplateParameter(std::string name);
		void RegisterSpecialization(std::vector<std::string> typeArgs, std::unique_ptr<Class> specializedClass);

	private:
		[[nodiscard]] cct::refl::Object* GetMemberVariable(std::size_t index, const cct::refl::Object& self) const override;
		[[nodiscard]] void* GetNativeMemberVariable(std::size_t index, const cct::refl::Object& self) const override;
		virtual void Initialize() override;

		std::vector<std::string> m_templateParameters;
		std::unordered_map<std::string /*typeArgsKey*/, Class* /*specializedClass*/> m_specializations;

		static constexpr std::string_view SpecializationSeparator = ",";
		std::string MakeSpecializationKey(std::span<std::string_view> typeArgs) const;
	};
} // namespace cct::refl

#include "Concerto/Reflection/TemplateClass/TemplateClass.inl"

#endif // CONCERTO_REFLECTION_TEMPLATE_CLASS_HPP
