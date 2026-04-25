//
// Created by arthur on 16/12/2025
//

#include "Concerto/Reflection/GenericClass/GenericClass.hpp"

namespace cct::refl
{
	GenericClass::GenericClass(Namespace* nameSpace, std::string name, const Class* baseClass) :
		Class(nameSpace, std::move(name), baseClass),
		m_typeParameterCount(0)
	{
	}

	std::size_t GenericClass::GetTypeParameterCount() const
	{
		return m_typeParameterCount;
	}

	std::span<const std::string> GenericClass::GetTypeParameterNames() const
	{
		return m_typeParameterNames;
	}

	std::unique_ptr<Object> GenericClass::CreateDefaultObject() const
	{
		CCT_ASSERT_FALSE("Cannot instantiate generic class template directly. Use CreateDefaultObject(std::span<const Class*>)");
		return nullptr;
	}

	std::unique_ptr<Object> GenericClass::CreateDefaultObject(
		std::span<const Class*> typeArgs) const
	{
		if (typeArgs.size() != m_typeParameterCount)
		{
			CCT_ASSERT_FALSE("Type argument count mismatch for '{}': expected {}, got {}",
							 GetName(), m_typeParameterCount, typeArgs.size());
			return nullptr;
		}

		for (std::size_t i = 0; i < typeArgs.size(); ++i)
		{
			if (!typeArgs[i])
			{
				CCT_ASSERT_FALSE("Type argument {} for '{}' is nullptr", i, GetName());
				return nullptr;
			}
		}

		CCT_ASSERT_FALSE("GenericClass::CreateDefaultObject(span) not implemented. This should be overridden in generated code.");
		return nullptr;
	}

	bool GenericClass::IsGenericClass() const
	{
		return true;
	}

	void GenericClass::AddTypeParameter(std::string fieldName)
	{
		m_typeParameterNames.push_back(std::move(fieldName));
	}

	void GenericClass::SetTypeParameterCount(std::size_t count)
	{
		m_typeParameterCount = count;
	}

	void GenericClass::Initialize()
	{
		// Base implementation does nothing
		// Subclasses override this to set up type parameters and other metadata
	}
} // namespace cct::refl
