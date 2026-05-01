#include "Concerto/Reflection/Json/Json.hpp"

#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

#include <simdjson.h>

#include <Concerto/Core/TypeInfo/TypeInfo.hpp>

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Enumeration/Enumeration.refl.hpp"
#include "Concerto/Reflection/Enumeration/EnumerationClass.hpp"
#include "Concerto/Reflection/EnumValue/EnumValue.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"
#include "Concerto/Reflection/Vector/Vector.refl.hpp"

namespace cct::refl
{
	namespace
	{
		void SerializeObject(std::string& out, const Object& obj);
		bool DeserializeObjectFields(Object& target, simdjson::ondemand::object& jsonObj);

		void AppendEscapedString(std::string& out, std::string_view s)
		{
			out.reserve(out.size() + s.size() + 2);
			for (char c : s)
			{
				switch (c)
				{
					case '"':  out += "\\\""; break;
					case '\\': out += "\\\\"; break;
					case '\b': out += "\\b";  break;
					case '\f': out += "\\f";  break;
					case '\n': out += "\\n";  break;
					case '\r': out += "\\r";  break;
					case '\t': out += "\\t";  break;
					default:
						if (static_cast<unsigned char>(c) < 0x20)
						{
							char buf[8];
							std::snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
							out += buf;
						}
						else
						{
							out += c;
						}
						break;
				}
			}
		}

		std::string FormatFloat(float value)
		{
			std::stringstream oss;
			oss << std::fixed << std::setprecision(6) << value;
			std::string result = oss.str();
			if (result.find('.') != std::string::npos)
			{
				result.erase(result.find_last_not_of('0') + 1, std::string::npos);
				if (result.back() == '.')
					result.pop_back();
			}
			return result;
		}

		std::string FormatDouble(double value)
		{
			std::stringstream oss;
			oss << std::fixed << std::setprecision(15) << value;
			std::string result = oss.str();
			if (result.find('.') != std::string::npos)
			{
				result.erase(result.find_last_not_of('0') + 1, std::string::npos);
				if (result.back() == '.')
					result.pop_back();
			}
			return result;
		}

		bool TryEmitPrimitiveWrapper(std::string& out, const Object& memberObj)
		{
			const Class* klass = memberObj.GetDynamicClass();
			if (!klass)
				return false;
			const std::string_view name = klass->GetName();

			const NativeMemberVariable* mv = klass->GetNativeMemberVariable("m_value");
			if (!mv)
				return false;
			const std::size_t idx = mv->GetIndex();

			if (name == "Boolean")
			{
				const bool* p = memberObj.GetNativeMemberVariable<bool>(idx);
				out += (p && *p) ? "true" : "false";
				return true;
			}
			if (name == "Int8")
			{
				const cct::Int8* p = memberObj.GetNativeMemberVariable<cct::Int8>(idx);
				out += std::to_string(p ? static_cast<int>(*p) : 0);
				return true;
			}
			if (name == "Int16")
			{
				const cct::Int16* p = memberObj.GetNativeMemberVariable<cct::Int16>(idx);
				out += std::to_string(p ? static_cast<int>(*p) : 0);
				return true;
			}
			if (name == "Int32")
			{
				const cct::Int32* p = memberObj.GetNativeMemberVariable<cct::Int32>(idx);
				out += std::to_string(p ? *p : 0);
				return true;
			}
			if (name == "Int64")
			{
				const cct::Int64* p = memberObj.GetNativeMemberVariable<cct::Int64>(idx);
				out += std::to_string(p ? *p : 0);
				return true;
			}
			if (name == "UInt8")
			{
				const cct::UInt8* p = memberObj.GetNativeMemberVariable<cct::UInt8>(idx);
				out += std::to_string(p ? static_cast<unsigned>(*p) : 0u);
				return true;
			}
			if (name == "UInt16")
			{
				const cct::UInt16* p = memberObj.GetNativeMemberVariable<cct::UInt16>(idx);
				out += std::to_string(p ? static_cast<unsigned>(*p) : 0u);
				return true;
			}
			if (name == "UInt32")
			{
				const cct::UInt32* p = memberObj.GetNativeMemberVariable<cct::UInt32>(idx);
				out += std::to_string(p ? *p : 0u);
				return true;
			}
			if (name == "UInt64")
			{
				const cct::UInt64* p = memberObj.GetNativeMemberVariable<cct::UInt64>(idx);
				out += std::to_string(p ? *p : 0ull);
				return true;
			}
			if (name == "Float32")
			{
				const float* p = memberObj.GetNativeMemberVariable<float>(idx);
				out += FormatFloat(p ? *p : 0.0F);
				return true;
			}
			if (name == "Float64")
			{
				const double* p = memberObj.GetNativeMemberVariable<double>(idx);
				out += FormatDouble(p ? *p : 0.0);
				return true;
			}
			if (name == "String")
			{
				const std::string* p = memberObj.GetNativeMemberVariable<std::string>(idx);
				out += "\"";
				if (p)
					AppendEscapedString(out, *p);
				out += "\"";
				return true;
			}
			return false;
		}

		bool TryAssignPrimitiveWrapper(Object& memberObj, simdjson::ondemand::value& field)
		{
			const Class* klass = memberObj.GetDynamicClass();
			if (!klass)
				return false;
			const std::string_view name = klass->GetName();

			const NativeMemberVariable* mv = klass->GetNativeMemberVariable("m_value");
			if (!mv)
				return false;
			const std::size_t idx = mv->GetIndex();

			auto setInt = [&](auto* dst) {
				int64_t v;
				if (field.get(v) == simdjson::SUCCESS && dst)
					*dst = static_cast<std::remove_reference_t<decltype(*dst)>>(v);
			};
			auto setUInt = [&](auto* dst) {
				uint64_t v;
				if (field.get(v) == simdjson::SUCCESS && dst)
					*dst = static_cast<std::remove_reference_t<decltype(*dst)>>(v);
			};

			if (name == "Boolean")
			{
				bool v;
				if (field.get(v) == simdjson::SUCCESS)
					if (bool* p = memberObj.GetNativeMemberVariable<bool>(idx))
						*p = v;
				return true;
			}
			if (name == "Int8")  { setInt(memberObj.GetNativeMemberVariable<cct::Int8>(idx));  return true; }
			if (name == "Int16") { setInt(memberObj.GetNativeMemberVariable<cct::Int16>(idx)); return true; }
			if (name == "Int32") { setInt(memberObj.GetNativeMemberVariable<cct::Int32>(idx)); return true; }
			if (name == "Int64") { setInt(memberObj.GetNativeMemberVariable<cct::Int64>(idx)); return true; }
			if (name == "UInt8")  { setUInt(memberObj.GetNativeMemberVariable<cct::UInt8>(idx));  return true; }
			if (name == "UInt16") { setUInt(memberObj.GetNativeMemberVariable<cct::UInt16>(idx)); return true; }
			if (name == "UInt32") { setUInt(memberObj.GetNativeMemberVariable<cct::UInt32>(idx)); return true; }
			if (name == "UInt64") { setUInt(memberObj.GetNativeMemberVariable<cct::UInt64>(idx)); return true; }
			if (name == "Float32")
			{
				double v;
				if (field.get(v) == simdjson::SUCCESS)
					if (float* p = memberObj.GetNativeMemberVariable<float>(idx))
						*p = static_cast<float>(v);
				return true;
			}
			if (name == "Float64")
			{
				double v;
				if (field.get(v) == simdjson::SUCCESS)
					if (double* p = memberObj.GetNativeMemberVariable<double>(idx))
						*p = v;
				return true;
			}
			if (name == "String")
			{
				std::string_view v;
				if (field.get(v) == simdjson::SUCCESS)
					if (std::string* p = memberObj.GetNativeMemberVariable<std::string>(idx))
						*p = std::string(v);
				return true;
			}
			return false;
		}

		// Serialize one member's value: primitive flat, Vector array, Enumeration name, or nested object.
		void SerializeMemberValue(std::string& out, const Object& memberObj)
		{
			if (TryEmitPrimitiveWrapper(out, memberObj))
				return;

			if (const auto* enumObj = dynamic_cast<const Enumeration*>(&memberObj))
			{
				// Emit a structured enum value with metadata so the UI can render a dropdown.
				// Format: {"$enum": "<EnumName>", "value": "<currentName>", "options": ["A", "B", ...]}
				const auto* enumClass = dynamic_cast<const EnumerationClass*>(enumObj->GetDynamicClass());

				out += "{\"$enum\":\"";
				if (enumClass)
					AppendEscapedString(out, enumClass->GetName());
				out += "\",\"value\":\"";
				AppendEscapedString(out, enumObj->ToString());
				out += "\",\"options\":[";
				if (enumClass)
				{
					bool firstOpt = true;
					for (const auto& ev : enumClass->GetEnumValues())
					{
						if (!ev)
							continue;
						if (!firstOpt)
							out += ",";
						firstOpt = false;
						out += "\"";
						AppendEscapedString(out, ev->GetName());
						out += "\"";
					}
				}
				out += "]}";
				return;
			}

			if (const auto* vec = dynamic_cast<const Vector*>(&memberObj))
			{
				out += "[";
				const std::size_t n = vec->GetCount();
				for (std::size_t i = 0; i < n; ++i)
				{
					if (i > 0)
						out += ",";
					if (const Object* elem = vec->Get(i))
						SerializeObject(out, *elem);
					else
						out += "null";
				}
				out += "]";
				return;
			}

			// Nested non-primitive Object — recurse.
			SerializeObject(out, memberObj);
		}

		void SerializeObject(std::string& out, const Object& obj)
		{
			const Class* klass = obj.GetDynamicClass();
			if (!klass)
			{
				out += "{}";
				return;
			}

			out += "{";
			bool first = true;

			// Native members (raw int/float/bool/std::string fields)
			for (const auto& member : klass->GetNativeMemberVariables())
			{
				if (!member)
					continue;

				if (!first)
					out += ",";
				first = false;

				out += "\"";
				AppendEscapedString(out, member->GetName());
				out += "\":";

				const std::size_t idx = member->GetIndex();
				const UInt64 typeId = member->GetTypeId();

				if (typeId == cct::TypeId<int>())
				{
					const int* p = obj.GetNativeMemberVariable<int>(idx);
					out += std::to_string(p ? *p : 0);
				}
				else if (typeId == cct::TypeId<float>())
				{
					const float* p = obj.GetNativeMemberVariable<float>(idx);
					out += FormatFloat(p ? *p : 0.0F);
				}
				else if (typeId == cct::TypeId<bool>())
				{
					const bool* p = obj.GetNativeMemberVariable<bool>(idx);
					out += (p && *p) ? "true" : "false";
				}
				else if (typeId == cct::TypeId<std::string>())
				{
					const std::string* p = obj.GetNativeMemberVariable<std::string>(idx);
					out += "\"";
					if (p)
						AppendEscapedString(out, *p);
					out += "\"";
				}
				else
				{
					out += "null";
				}
			}

			// Reflected Object members (primitive wrappers, Vectors, nested Objects).
			for (const auto& member : klass->GetMemberVariables())
			{
				if (!member)
					continue;

				const std::size_t idx = member->GetIndex();
				const Object* memberObj = klass->GetMemberVariable(idx, obj);
				if (!memberObj)
					continue;

				if (!first)
					out += ",";
				first = false;

				out += "\"";
				AppendEscapedString(out, member->GetName());
				out += "\":";

				SerializeMemberValue(out, *memberObj);
			}

			out += "}";
		}

		bool DeserializeObjectFields(Object& target, simdjson::ondemand::object& jsonObj)
		{
			const Class* klass = target.GetDynamicClass();
			if (!klass)
				return false;

			for (auto field : jsonObj)
			{
				std::string_view key;
				if (field.unescaped_key().get(key))
					continue;

				// 1. Native members
				if (const NativeMemberVariable* nm = klass->GetNativeMemberVariable(key))
				{
					const std::size_t idx = nm->GetIndex();
					const UInt64 typeId = nm->GetTypeId();
					simdjson::ondemand::value v = field.value();

					if (typeId == cct::TypeId<int>())
					{
						int64_t iv;
						if (v.get(iv) == simdjson::SUCCESS)
							if (int* p = target.GetNativeMemberVariable<int>(idx))
								*p = static_cast<int>(iv);
					}
					else if (typeId == cct::TypeId<float>())
					{
						double fv;
						if (v.get(fv) == simdjson::SUCCESS)
							if (float* p = target.GetNativeMemberVariable<float>(idx))
								*p = static_cast<float>(fv);
					}
					else if (typeId == cct::TypeId<bool>())
					{
						bool bv;
						if (v.get(bv) == simdjson::SUCCESS)
							if (bool* p = target.GetNativeMemberVariable<bool>(idx))
								*p = bv;
					}
					else if (typeId == cct::TypeId<std::string>())
					{
						std::string_view sv;
						if (v.get(sv) == simdjson::SUCCESS)
							if (std::string* p = target.GetNativeMemberVariable<std::string>(idx))
								*p = std::string(sv);
					}
					continue;
				}

				// 2. Reflected member (primitive wrapper, Vector, or nested Object).
				if (!klass->HasMemberVariable(key))
					continue;

				const MemberVariable* m = klass->GetMemberVariable(key);
				if (!m)
					continue;
				Object* memberObj = klass->GetMemberVariable(m->GetIndex(), target);
				if (!memberObj)
					continue;

				simdjson::ondemand::value v = field.value();
				if (TryAssignPrimitiveWrapper(*memberObj, v))
					continue;

				if (auto* enumObj = dynamic_cast<Enumeration*>(memberObj))
				{
					// Accept "Normal" (name string), 0 (raw int), or {value:"Normal", ...} object.
					std::string_view nameView;
					if (v.get(nameView) == simdjson::SUCCESS)
					{
						enumObj->SetEnumValue(enumObj->FromString(nameView));
					}
					else
					{
						int64_t intVal;
						if (v.get(intVal) == simdjson::SUCCESS)
						{
							enumObj->SetEnumValue(intVal);
						}
						else
						{
							simdjson::ondemand::object enumJsonObj;
							if (v.get_object().get(enumJsonObj) == simdjson::SUCCESS)
							{
								std::string_view innerName;
								if (enumJsonObj["value"].get(innerName) == simdjson::SUCCESS)
									enumObj->SetEnumValue(enumObj->FromString(innerName));
							}
						}
					}
					continue;
				}

				if (auto* vec = dynamic_cast<Vector*>(memberObj))
				{
					simdjson::ondemand::array arr;
					if (v.get(arr) != simdjson::SUCCESS)
						continue;

					vec->Clear();
					for (auto elemVal : arr)
					{
						simdjson::ondemand::object elemObj;
						if (elemVal.get_object().get(elemObj) != simdjson::SUCCESS)
							continue;
						Object* newElem = vec->Add();
						if (newElem)
							DeserializeObjectFields(*newElem, elemObj);
					}
					continue;
				}

				// Nested non-primitive Object — recurse.
				simdjson::ondemand::object nestedObj;
				if (v.get_object().get(nestedObj) == simdjson::SUCCESS)
					DeserializeObjectFields(*memberObj, nestedObj);
			}
			return true;
		}
	}

	std::string Json::ToJson(const Object& obj)
	{
		std::string out;
		SerializeObject(out, obj);
		return out;
	}

	bool Json::FromJson(Object& target, std::string_view json)
	{
		simdjson::ondemand::parser parser;
		simdjson::padded_string padded(json.data(), json.size());

		simdjson::ondemand::document doc;
		if (parser.iterate(padded).get(doc))
			return false;

		simdjson::ondemand::object obj;
		if (doc.get_object().get(obj))
			return false;

		return DeserializeObjectFields(target, obj);
	}
} // namespace cct::refl
