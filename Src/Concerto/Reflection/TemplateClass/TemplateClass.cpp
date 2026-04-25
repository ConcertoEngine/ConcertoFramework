//
// Created by arthur on 10/12/2024.
//

#include "Concerto/Reflection/TemplateClass/TemplateClass.hpp"

#include "Concerto/Reflection/Namespace/Namespace.hpp"

namespace cct::refl
{
	TemplateClass::TemplateClass(Namespace* nameSpace, std::string name, const Class* baseClass) :
		Class(nameSpace, std::move(name), baseClass)
	{
	}

	std::span<const std::string> TemplateClass::GetTemplateParameters() const
	{
		return m_templateParameters;
	}

	const Class* TemplateClass::GetSpecialization(std::span<std::string_view> typeArgs) const
	{
		const auto key = MakeSpecializationKey(typeArgs);
		const auto it = m_specializations.find(key);
		if (it != m_specializations.end())
		{
			return it->second;
		}
		return nullptr;
	}

	const Class* TemplateClass::GetSpecialization(std::string_view typeArg) const
	{
		std::vector<std::string_view> typeArgs;
		typeArgs.push_back(typeArg);
		return GetSpecialization(std::span<std::string_view>(typeArgs));
	}

	bool TemplateClass::HasSpecialization(std::span<std::string_view> typeArgs) const
	{
		return GetSpecialization(typeArgs) != nullptr;
	}

	bool TemplateClass::IsTemplateClass() const
	{
		return true;
	}

	std::unique_ptr<cct::refl::Object> TemplateClass::CreateDefaultObject() const
	{
		// Template classes cannot be instantiated directly
		// Must use a specialization
		return nullptr;
	}

	void TemplateClass::AddTemplateParameter(std::string name)
	{
		m_templateParameters.push_back(std::move(name));
	}

	void TemplateClass::RegisterSpecialization(std::vector<std::string> typeArgs, std::unique_ptr<Class> specializedClass)
	{
		std::vector<std::string_view> typeArgsView(typeArgs.begin(), typeArgs.end());
		const auto key = MakeSpecializationKey(typeArgsView);
		m_specializations[key] = specializedClass.release();
	}

	std::string TemplateClass::MakeSpecializationKey(std::span<std::string_view> typeArgs) const
	{
		std::string key;
		for (std::size_t i = 0; i < typeArgs.size(); ++i)
		{
			key += typeArgs[i];
			if (i < typeArgs.size() - 1)
			{
				key += SpecializationSeparator;
			}
		}
		return key;
	}

	cct::refl::Object* TemplateClass::GetMemberVariable(std::size_t index, const cct::refl::Object& self) const
	{
		// Template classes don't have member access
		return nullptr;
	}

	void* TemplateClass::GetNativeMemberVariable(std::size_t index, const cct::refl::Object& self) const
	{
		// Template classes don't have native member access
		return nullptr;
	}

	void TemplateClass::Initialize()
	{
		// Initialize template class
	}
} // namespace cct::refl
