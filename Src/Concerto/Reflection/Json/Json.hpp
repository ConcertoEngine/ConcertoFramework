#ifndef CONCERTO_REFLECTION_JSON_HPP
#define CONCERTO_REFLECTION_JSON_HPP

#include <string>
#include <string_view>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Object;
	class Registry;

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
		/** Serialize all native members of obj into a flat JSON object string.
		 *  `emitClass` (default true) writes the `"class"` discriminator on the
		 *  root and every nested object / vector element. Pass false for DTO
		 *  dumps (catalog entries, route lists) whose wrapper type is not part
		 *  of the payload. Keep the default for polymorphic graphs (Scene.nodes). */
		static std::string ToJson(const Object& obj, bool emitClass = true);

		/** Serialize obj to a JSON file at path. Returns false on I/O error. */
		static bool ToJsonFile(const Object& obj, const std::string& path);

		/**
		 * Parse a JSON object string and assign matching native members of target.
		 * Unknown keys and unsupported types are silently skipped. Returns false
		 * only on parse error (malformed JSON).
		 *
		 * Polymorphic Vector elements encountered while parsing are created through
		 * `registry` (Registry::Allocate) when one is given, so they end up tracked in
		 * it exactly as if the caller had called registry.Allocate() by hand. With no
		 * registry (the default), created objects are plain klass->CreateDefaultObject()
		 * results and are not tracked anywhere.
		 */
		static bool FromJson(Object& target, std::string_view json, Registry* registry = nullptr);

		/** Deserialize obj from a JSON file at path. Returns false on I/O or parse error. */
		static bool FromJsonFile(Object& target, const std::string& path, Registry* registry = nullptr);
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_JSON_HPP
