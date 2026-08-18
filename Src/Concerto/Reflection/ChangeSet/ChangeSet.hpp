//
// Created by arthur on 18/08/2026.
//

#ifndef CONCERTO_REFLECTION_CHANGESET_HPP
#define CONCERTO_REFLECTION_CHANGESET_HPP

#include <string>
#include <string_view>
#include <vector>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Object;

	struct FieldChange
	{
		std::string m_ownerUuid;
		std::string m_path;
		std::string m_value;
	};

	struct ChangeSet
	{
		std::vector<FieldChange> m_changes;
		bool m_structural = false;

		[[nodiscard]] bool IsEmpty() const
		{
			return m_changes.empty() && !m_structural;
		}
	};

	class CCT_REFLECTION_API ChangeCollector
	{
	public:
		static ChangeSet Collect(Object& root);
		static void ClearDirty(Object& root);
		static Object* FindByUuid(Object& root, std::string_view uuid);
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_CHANGESET_HPP
