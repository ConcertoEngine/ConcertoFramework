#ifndef CONCERTO_REFLECTION_JSON_HPP
#define CONCERTO_REFLECTION_JSON_HPP

#include <string>
#include <string_view>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Object;

	/**
	 * Reflection-driven JSON serializer for cct::refl::Object instances.
	 *
	 * Iterates the object's native members (RBE_UI_NATIVE_MEMBER) and maps each
	 * field by TypeId. Supported native types: int, float, bool, std::string.
	 * Other types are emitted as null (ToJson) or skipped (FromJson).
	 *
	 * Usage:
	 *     std::string s = cct::refl::Json::ToJson(myObject);
	 *
	 *     auto obj = MaClasse::GetClass()->CreateDefaultObject<MaClasse>();
	 *     bool ok = cct::refl::Json::FromJson(*obj, R"({"intValue":42})");
	 */
	class CCT_REFLECTION_API Json
	{
	public:
		/** Serialize all native members of obj into a flat JSON object string. */
		static std::string ToJson(const Object& obj);

		/**
		 * Parse a JSON object string and assign matching native members of target.
		 * Unknown keys and unsupported types are silently skipped. Returns false
		 * only on parse error (malformed JSON).
		 */
		static bool FromJson(Object& target, std::string_view json);
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_JSON_HPP
