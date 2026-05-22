#ifndef CONCERTO_REFLECTION_FIELD_VISITOR_HPP
#define CONCERTO_REFLECTION_FIELD_VISITOR_HPP

#include <optional>
#include <string>
#include <string_view>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Object;
	class Boolean;
	class Int8;
	class Int16;
	class Int32;
	class Int64;
	class UInt8;
	class UInt16;
	class UInt32;
	class UInt64;
	class Float32;
	class Float64;
	class String;
	class File;
	class Folder;
	class Enumeration;
	class Vector;

	/// Format-agnostic visitor over all fields of a reflected Object.
	///
	/// Implement a subclass to produce/consume any serialization format without
	/// Object or its subclasses knowing about the format.
	///
	/// All Visit overloads have a no-op default body so implementers only override
	/// what they need. PushMeta is called (once) before each field that carries
	/// Min/Max/integer attributes; it may be ignored by visitors that don't care.
	class CCT_REFLECTION_API FieldVisitor
	{
	public:
		virtual ~FieldVisitor() = default;

		// Native C++ types
		virtual void Visit(std::string_view name, int& v)         {}
		virtual void Visit(std::string_view name, float& v)       {}
		virtual void Visit(std::string_view name, bool& v)        {}
		virtual void Visit(std::string_view name, std::string& v) {}

		// Primitive wrappers
		virtual void Visit(std::string_view name, Boolean& v)  {}
		virtual void Visit(std::string_view name, Int8& v)     {}
		virtual void Visit(std::string_view name, Int16& v)    {}
		virtual void Visit(std::string_view name, Int32& v)    {}
		virtual void Visit(std::string_view name, Int64& v)    {}
		virtual void Visit(std::string_view name, UInt8& v)    {}
		virtual void Visit(std::string_view name, UInt16& v)   {}
		virtual void Visit(std::string_view name, UInt32& v)   {}
		virtual void Visit(std::string_view name, UInt64& v)   {}
		virtual void Visit(std::string_view name, Float32& v)  {}
		virtual void Visit(std::string_view name, Float64& v)  {}
		virtual void Visit(std::string_view name, String& v)   {}
		virtual void Visit(std::string_view name, File& v)     {}
		virtual void Visit(std::string_view name, Folder& v)   {}

		// Containers and nested objects
		virtual void Visit(std::string_view name, Enumeration& v) {}
		virtual void Visit(std::string_view name, Vector& v)      {}
		virtual void Visit(std::string_view name, Object& v)      {}

		/// Called before a field that carries Min/Max/integer metadata.
		/// Visitors that render UI constraints (e.g. sliders) should override this.
		virtual void PushMeta(std::string_view name, std::optional<double> min,
		                      std::optional<double> max, bool isInteger) {}
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_FIELD_VISITOR_HPP
