//
// Created by arthur on 06/10/2025.
//

#ifndef CONCERTO_REFLECTION_REGISTRY_HPP
#define CONCERTO_REFLECTION_REGISTRY_HPP

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Object;
	class Class;
	enum class ChangeType;

	class CCT_REFLECTION_API Registry
	{
	public:
		explicit Registry(std::size_t defaultObjectCount = 0xFFF);

		Registry(Registry&&) noexcept = default;
		Registry(const Registry&) = delete;

		void NotifyPreChange(Object& changedObject, ChangeType changeType, std::string_view variableName);
		void NotifyPostChange(Object& changedObject, ChangeType changeType, std::string_view variableName);

		Object* Allocate(const Class* klass);
		void DeAllocate(const Object& object);

	private:
		std::size_t m_defaultObjectCount;
		std::unordered_map<const Class*, std::vector<std::unique_ptr<Object>>> m_objects; // TODO: remove std::unique_ptr
	};
} // namespace cct::refl

#include "Concerto/Reflection/Registry/Registry.inl"

#endif // CONCERTO_REFLECTION_REGISTRY_HPP