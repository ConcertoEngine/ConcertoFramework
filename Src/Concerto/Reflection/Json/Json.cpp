#include "Concerto/Reflection/Json/Json.hpp"

#include <fstream>
#include <map>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "Concerto/Reflection/Boolean/Boolean.refl.hpp"
#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/Enumeration/Enumeration.refl.hpp"
#include "Concerto/Reflection/Enumeration/EnumerationClass.hpp"
#include "Concerto/Reflection/EnumValue/EnumValue.hpp"
#include "Concerto/Reflection/FieldVisitor/FieldVisitor.hpp"
#include "Concerto/Reflection/File/File.refl.hpp"
#include "Concerto/Reflection/Float32/Float32.refl.hpp"
#include "Concerto/Reflection/Float64/Float64.refl.hpp"
#include "Concerto/Reflection/Folder/Folder.refl.hpp"
#include "Concerto/Reflection/Int16/Int16.refl.hpp"
#include "Concerto/Reflection/Int32/Int32.refl.hpp"
#include "Concerto/Reflection/Int64/Int64.refl.hpp"
#include "Concerto/Reflection/Int8/Int8.refl.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"
#include "Concerto/Reflection/String/String.refl.hpp"
#include "Concerto/Reflection/UInt16/UInt16.refl.hpp"
#include "Concerto/Reflection/UInt32/UInt32.refl.hpp"
#include "Concerto/Reflection/UInt64/UInt64.refl.hpp"
#include "Concerto/Reflection/UInt8/UInt8.refl.hpp"
#include "Concerto/Reflection/Vector/Vector.refl.hpp"
#include <nlohmann/json.hpp>

namespace cct::refl
{
	namespace
	{
		nlohmann::ordered_json SerializeObject(const Object& obj);

		struct JsonSerializer : FieldVisitor
		{
			nlohmann::ordered_json& out;

			explicit JsonSerializer(nlohmann::ordered_json& o) :
				out(o)
			{
			}

			void Visit(std::string_view name, int& v) override
			{
				out[std::string(name)] = v;
			}
			void Visit(std::string_view name, float& v) override
			{
				out[std::string(name)] = v;
			}
			void Visit(std::string_view name, bool& v) override
			{
				out[std::string(name)] = v;
			}
			void Visit(std::string_view name, std::string& v) override
			{
				out[std::string(name)] = v;
			}
			void Visit(std::string_view name, Boolean& v) override
			{
				out[std::string(name)] = v.Get();
			}
			void Visit(std::string_view name, Int8& v) override
			{
				out[std::string(name)] = v.Get();
			}
			void Visit(std::string_view name, Int16& v) override
			{
				out[std::string(name)] = v.Get();
			}
			void Visit(std::string_view name, Int32& v) override
			{
				out[std::string(name)] = v.Get();
			}
			void Visit(std::string_view name, Int64& v) override
			{
				out[std::string(name)] = v.Get();
			}
			void Visit(std::string_view name, UInt8& v) override
			{
				out[std::string(name)] = v.Get();
			}
			void Visit(std::string_view name, UInt16& v) override
			{
				out[std::string(name)] = v.Get();
			}
			void Visit(std::string_view name, UInt32& v) override
			{
				out[std::string(name)] = v.Get();
			}
			void Visit(std::string_view name, UInt64& v) override
			{
				out[std::string(name)] = v.Get();
			}
			void Visit(std::string_view name, Float32& v) override
			{
				out[std::string(name)] = v.Get();
			}
			void Visit(std::string_view name, Float64& v) override
			{
				out[std::string(name)] = v.Get();
			}
			void Visit(std::string_view name, String& v) override
			{
				out[std::string(name)] = v.Get();
			}

			void Visit(std::string_view name, File& v) override
			{
				out[std::string(name)] = {{"$file", "File"}, {"path", v.Get()}};
			}
			void Visit(std::string_view name, Folder& v) override
			{
				out[std::string(name)] = {{"$folder", "Folder"}, {"path", v.Get()}};
			}
			void Visit(std::string_view name, Enumeration& v) override
			{
				const auto* enumClass = dynamic_cast<const EnumerationClass*>(v.GetDynamicClass());
				nlohmann::ordered_json enumJson;
				enumJson["$enum"] = enumClass ? enumClass->GetName() : "";
				enumJson["value"] = v.ToString();
				auto opts = nlohmann::ordered_json::array();
				if (enumClass)
					for (const auto& ev : enumClass->GetEnumValues())
						if (ev)
							opts.push_back(ev->GetName());
				enumJson["options"] = std::move(opts);
				out[std::string(name)] = std::move(enumJson);
			}
			void Visit(std::string_view name, Vector& v) override
			{
				auto arr = nlohmann::ordered_json::array();
				for (std::size_t i = 0; i < v.GetCount(); ++i)
				{
					if (Object* elem = v.Get(i))
						arr.push_back(SerializeObject(*elem));
					else
						arr.push_back(nullptr);
				}
				out[std::string(name)] = std::move(arr);
			}
			void Visit(std::string_view name, Object& v) override
			{
				out[std::string(name)] = SerializeObject(v);
			}
		};

		nlohmann::ordered_json BuildMeta(const std::vector<const Class*>& chain)
		{
			nlohmann::ordered_json meta = nlohmann::ordered_json::object();

			auto processVar = [&](std::string_view fieldName, const auto* mv)
			{
				const auto& attrs = mv->GetAttributes();
				std::string minScalar, maxScalar;
				std::map<std::string, std::string> minMap, maxMap;
				for (const auto& [key, val] : attrs)
				{
					if (key == "Min")
						minScalar = val;
					else if (key == "Max")
						maxScalar = val;
					else if (key.size() > 4 && key.substr(0, 4) == "Min.")
						minMap[key.substr(4)] = val;
					else if (key.size() > 4 && key.substr(0, 4) == "Max.")
						maxMap[key.substr(4)] = val;
				}

				nlohmann::ordered_json entry = nlohmann::ordered_json::object();

				auto addScalarOrMap = [&](const char* key,
										  const std::string& scalar,
										  const std::map<std::string, std::string>& map)
				{
					if (scalar.empty() && map.empty())
						return;
					if (!scalar.empty())
					{
						nlohmann::ordered_json val = nlohmann::ordered_json::parse(scalar, nullptr, false);
						if (!val.is_discarded())
							entry[key] = std::move(val);
					}
					else
					{
						nlohmann::ordered_json mapJson = nlohmann::ordered_json::object();
						for (const auto& [k, v] : map)
						{
							nlohmann::ordered_json val = nlohmann::ordered_json::parse(v, nullptr, false);
							if (!val.is_discarded())
								mapJson[k] = std::move(val);
						}
						entry[key] = std::move(mapJson);
					}
				};

				addScalarOrMap("min", minScalar, minMap);
				addScalarOrMap("max", maxScalar, maxMap);

				if constexpr (std::is_same_v<std::remove_pointer_t<decltype(mv)>, MemberVariable>)
				{
					static constexpr std::string_view INTEGER_TYPES[] = {
						"Int8",
						"Int16",
						"Int32",
						"Int64",
						"UInt8",
						"UInt16",
						"UInt32",
						"UInt64",
					};
					if (const Class* type = mv->GetType())
					{
						for (const auto& intType : INTEGER_TYPES)
						{
							if (type->GetName() == intType)
							{
								entry["integer"] = true;
								break;
							}
						}
					}
				}

				if (!entry.empty())
					meta[std::string(fieldName)] = std::move(entry);
			};

			for (auto it = chain.rbegin(); it != chain.rend(); ++it)
			{
				const Class* c = *it;
				for (const auto& mv : c->GetNativeMemberVariables())
					if (mv)
						processVar(mv->GetName(), mv.get());
				for (const auto& mv : c->GetMemberVariables())
					if (mv)
						processVar(mv->GetName(), mv.get());
			}

			return meta;
		}

		nlohmann::ordered_json SerializeObject(const Object& obj)
		{
			const Class* klass = obj.GetDynamicClass();
			if (klass == nullptr)
				return nlohmann::ordered_json::object();

			std::vector<const Class*> chain;
			for (const Class* c = klass; c != nullptr && c->GetName() != "Object"; c = c->GetBaseClass())
				chain.push_back(c);

			nlohmann::ordered_json j = nlohmann::ordered_json::object();

			if (klass->GetName() != "Object")
				j["class"] = klass->GetFullyQualifiedName();

			JsonSerializer serializer{j};
			obj.Accept(serializer);

			nlohmann::ordered_json meta = BuildMeta(chain);
			if (!meta.empty())
				j["__meta__"] = std::move(meta);

			return j;
		}

		struct JsonDeserializer : FieldVisitor
		{
			const nlohmann::ordered_json& obj;

			explicit JsonDeserializer(const nlohmann::ordered_json& o) :
				obj(o)
			{
			}

			template<typename T>
			bool TryGet(std::string_view name, T& out) const
			{
				const auto it = obj.find(std::string(name));
				if (it == obj.end())
					return false;
				try
				{
					out = it->get<T>();
					return true;
				}
				catch (...)
				{
					return false;
				}
			}

			void Visit(std::string_view name, int& v) override
			{
				TryGet(name, v);
			}
			void Visit(std::string_view name, float& v) override
			{
				TryGet(name, v);
			}
			void Visit(std::string_view name, bool& v) override
			{
				TryGet(name, v);
			}
			void Visit(std::string_view name, std::string& v) override
			{
				TryGet(name, v);
			}

			void Visit(std::string_view name, Boolean& v) override
			{
				bool b;
				if (TryGet(name, b))
					v.Set(b);
			}
			void Visit(std::string_view name, Int8& v) override
			{
				int64_t i;
				if (TryGet(name, i))
					v.Set(static_cast<cct::Int8>(i));
			}
			void Visit(std::string_view name, Int16& v) override
			{
				int64_t i;
				if (TryGet(name, i))
					v.Set(static_cast<cct::Int16>(i));
			}
			void Visit(std::string_view name, Int32& v) override
			{
				int64_t i;
				if (TryGet(name, i))
					v.Set(static_cast<cct::Int32>(i));
			}
			void Visit(std::string_view name, Int64& v) override
			{
				int64_t i;
				if (TryGet(name, i))
					v.Set(i);
			}
			void Visit(std::string_view name, UInt8& v) override
			{
				uint64_t u;
				if (TryGet(name, u))
					v.Set(static_cast<cct::UInt8>(u));
			}
			void Visit(std::string_view name, UInt16& v) override
			{
				uint64_t u;
				if (TryGet(name, u))
					v.Set(static_cast<cct::UInt16>(u));
			}
			void Visit(std::string_view name, UInt32& v) override
			{
				uint64_t u;
				if (TryGet(name, u))
					v.Set(static_cast<cct::UInt32>(u));
			}
			void Visit(std::string_view name, UInt64& v) override
			{
				uint64_t u;
				if (TryGet(name, u))
					v.Set(u);
			}
			void Visit(std::string_view name, Float32& v) override
			{
				float f;
				if (TryGet(name, f))
					v.Set(f);
			}
			void Visit(std::string_view name, Float64& v) override
			{
				double d;
				if (TryGet(name, d))
					v.Set(d);
			}
			void Visit(std::string_view name, String& v) override
			{
				std::string s;
				if (TryGet(name, s))
					v.Set(std::move(s));
			}

			void Visit(std::string_view name, File& v) override
			{
				const auto it = obj.find(std::string(name));
				if (it == obj.end() || !it->is_object())
					return;
				const auto pit = it->find("path");
				if (pit != it->end() && pit->is_string())
					v.Set(pit->get<std::string>());
			}
			void Visit(std::string_view name, Folder& v) override
			{
				const auto it = obj.find(std::string(name));
				if (it == obj.end() || !it->is_object())
					return;
				const auto pit = it->find("path");
				if (pit != it->end() && pit->is_string())
					v.Set(pit->get<std::string>());
			}
			void Visit(std::string_view name, Enumeration& v) override
			{
				const auto it = obj.find(std::string(name));
				if (it == obj.end())
					return;
				if (it->is_string())
				{
					v.SetEnumValue(v.FromString(it->get<std::string>()));
					return;
				}
				if (it->is_number_integer())
				{
					v.SetEnumValue(it->get<int64_t>());
					return;
				}
				if (it->is_object())
				{
					const auto vit = it->find("value");
					if (vit != it->end() && vit->is_string())
						v.SetEnumValue(v.FromString(vit->get<std::string>()));
				}
			}
			void Visit(std::string_view name, Vector& v) override
			{
				const auto it = obj.find(std::string(name));
				if (it == obj.end() || !it->is_array())
					return;

				v.Clear();
				ScopedObjectFlag constructing(v, ObjectFlags::Constructing,
											  [&v]()
											  { v.OnValueChanged.Emit(); });

				for (const auto& elemJson : *it)
				{
					if (!elemJson.is_object())
						continue;

					if (v.GetElementType() == nullptr)
					{
						const auto cit = elemJson.find("class");
						if (cit == elemJson.end() || !cit->is_string())
							continue;
						const Class* cls = GetClassByName(cit->get<std::string>());
						if (!cls)
							continue;
						auto newObj = cls->CreateDefaultObject();
						if (!newObj)
							continue;
						Object* elem = newObj.get();
						v.Add(std::move(newObj));
						JsonDeserializer elemDeser{elemJson};
						elem->Accept(elemDeser);
					}
					else
					{
						Object* elem = v.Add();
						if (elem)
						{
							JsonDeserializer elemDeser{elemJson};
							elem->Accept(elemDeser);
						}
					}
				}
			}
			void Visit(std::string_view name, Object& v) override
			{
				const auto it = obj.find(std::string(name));
				if (it == obj.end() || !it->is_object())
					return;
				JsonDeserializer nestedDeser{*it};
				v.Accept(nestedDeser);
			}
		};

	} // namespace

	std::string Json::ToJson(const Object& obj)
	{
		return SerializeObject(obj).dump();
	}

	bool Json::FromJson(Object& target, std::string_view json)
	{
		const nlohmann::ordered_json j = nlohmann::ordered_json::parse(json, nullptr, false);
		if (j.is_discarded() || !j.is_object())
			return false;
		JsonDeserializer deser{j};
		target.Accept(deser);
		return true;
	}

	bool Json::ToJsonFile(const Object& obj, const std::string& path)
	{
		std::ofstream file(path);
		if (!file.is_open())
			return false;
		file << SerializeObject(obj).dump(4);
		return file.good();
	}

	bool Json::FromJsonFile(Object& target, const std::string& path)
	{
		std::ifstream file(path);
		if (!file.is_open())
			return false;
		const nlohmann::ordered_json j = nlohmann::ordered_json::parse(file, nullptr, false);
		if (j.is_discarded() || !j.is_object())
			return false;
		JsonDeserializer deser{j};
		target.Accept(deser);
		return true;
	}

} // namespace cct::refl
