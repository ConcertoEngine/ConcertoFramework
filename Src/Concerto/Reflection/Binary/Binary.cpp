//
// Created by arthur on 18/08/2026.
//

#include "Concerto/Reflection/Binary/Binary.hpp"

#include <cstring>
#include <map>
#include <string>
#include <string_view>

#include <Concerto/Core/Uuid/Uuid.hpp>

#include "Concerto/Reflection/Boolean/Boolean.refl.hpp"
#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/Enumeration/Enumeration.refl.hpp"
#include "Concerto/Reflection/FieldVisitor/FieldVisitor.hpp"
#include "Concerto/Reflection/File/File.refl.hpp"
#include "Concerto/Reflection/Float32/Float32.refl.hpp"
#include "Concerto/Reflection/Float64/Float64.refl.hpp"
#include "Concerto/Reflection/Folder/Folder.refl.hpp"
#include "Concerto/Reflection/Int16/Int16.refl.hpp"
#include "Concerto/Reflection/Int32/Int32.refl.hpp"
#include "Concerto/Reflection/Int64/Int64.refl.hpp"
#include "Concerto/Reflection/Int8/Int8.refl.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"
#include "Concerto/Reflection/Registry/Registry.hpp"
#include "Concerto/Reflection/String/String.refl.hpp"
#include "Concerto/Reflection/UInt16/UInt16.refl.hpp"
#include "Concerto/Reflection/UInt32/UInt32.refl.hpp"
#include "Concerto/Reflection/UInt64/UInt64.refl.hpp"
#include "Concerto/Reflection/UInt8/UInt8.refl.hpp"
#include "Concerto/Reflection/Vector/Vector.refl.hpp"

namespace cct::refl
{
	namespace
	{
		enum class Tag : std::uint8_t
		{
			End = 0,
			NativeInt,
			NativeFloat,
			NativeBool,
			NativeString,
			Boolean,
			Int8,
			Int16,
			Int32,
			Int64,
			UInt8,
			UInt16,
			UInt32,
			UInt64,
			Float32,
			Float64,
			String,
			File,
			Folder,
			Enumeration,
			Vector,
			Object
		};

		constexpr std::string_view kClassKey = "class";
		constexpr std::string_view kUuidKey = "$uuid";

		class Writer
		{
		public:
			explicit Writer(std::vector<std::byte>& out) :
				m_out(out)
			{
			}

			void Raw(const void* data, std::size_t size)
			{
				const auto* bytes = static_cast<const std::byte*>(data);
				m_out.insert(m_out.end(), bytes, bytes + size);
			}

			template<typename T>
			void Pod(T value)
			{
				Raw(&value, sizeof(T));
			}

			void TagByte(Tag tag)
			{
				Pod(static_cast<std::uint8_t>(tag));
			}

			void Name(std::string_view name)
			{
				Pod(static_cast<std::uint16_t>(name.size()));
				Raw(name.data(), name.size());
			}

			void Text(std::string_view text)
			{
				Pod(static_cast<std::uint32_t>(text.size()));
				Raw(text.data(), text.size());
			}

		private:
			std::vector<std::byte>& m_out;
		};

		void WriteObject(Writer& writer, const Object& obj, bool emitClass);

		struct BinarySerializer : FieldVisitor
		{
			Writer& writer;
			bool emitClass;

			BinarySerializer(Writer& w, bool emitClass_) :
				writer(w),
				emitClass(emitClass_)
			{
			}

			template<typename T>
			void Scalar(std::string_view name, Tag tag, T value)
			{
				writer.TagByte(tag);
				writer.Name(name);
				writer.Pod(value);
			}

			void Textual(std::string_view name, Tag tag, std::string_view value)
			{
				writer.TagByte(tag);
				writer.Name(name);
				writer.Text(value);
			}

			void Visit(std::string_view name, int& v) override
			{
				Scalar(name, Tag::NativeInt, static_cast<std::int32_t>(v));
			}
			void Visit(std::string_view name, float& v) override
			{
				Scalar(name, Tag::NativeFloat, v);
			}
			void Visit(std::string_view name, bool& v) override
			{
				Scalar(name, Tag::NativeBool, static_cast<std::uint8_t>(v ? 1 : 0));
			}
			void Visit(std::string_view name, std::string& v) override
			{
				Textual(name, Tag::NativeString, v);
			}
			void Visit(std::string_view name, refl::Boolean& v) override
			{
				Scalar(name, Tag::Boolean, static_cast<std::uint8_t>(v.Get() ? 1 : 0));
			}
			void Visit(std::string_view name, refl::Int8& v) override
			{
				Scalar(name, Tag::Int8, v.Get());
			}
			void Visit(std::string_view name, refl::Int16& v) override
			{
				Scalar(name, Tag::Int16, v.Get());
			}
			void Visit(std::string_view name, refl::Int32& v) override
			{
				Scalar(name, Tag::Int32, v.Get());
			}
			void Visit(std::string_view name, refl::Int64& v) override
			{
				Scalar(name, Tag::Int64, v.Get());
			}
			void Visit(std::string_view name, refl::UInt8& v) override
			{
				Scalar(name, Tag::UInt8, v.Get());
			}
			void Visit(std::string_view name, refl::UInt16& v) override
			{
				Scalar(name, Tag::UInt16, v.Get());
			}
			void Visit(std::string_view name, refl::UInt32& v) override
			{
				Scalar(name, Tag::UInt32, v.Get());
			}
			void Visit(std::string_view name, refl::UInt64& v) override
			{
				Scalar(name, Tag::UInt64, v.Get());
			}
			void Visit(std::string_view name, refl::Float32& v) override
			{
				Scalar(name, Tag::Float32, v.Get());
			}
			void Visit(std::string_view name, refl::Float64& v) override
			{
				Scalar(name, Tag::Float64, v.Get());
			}
			void Visit(std::string_view name, refl::String& v) override
			{
				Textual(name, Tag::String, v.Get());
			}
			void Visit(std::string_view name, refl::File& v) override
			{
				Textual(name, Tag::File, v.Get());
			}
			void Visit(std::string_view name, refl::Folder& v) override
			{
				Textual(name, Tag::Folder, v.Get());
			}
			void Visit(std::string_view name, refl::Enumeration& v) override
			{
				writer.TagByte(Tag::Enumeration);
				writer.Name(name);
				writer.Pod(static_cast<std::int64_t>(v.GetValue()));
			}
			void Visit(std::string_view name, refl::Vector& v) override
			{
				writer.TagByte(Tag::Vector);
				writer.Name(name);
				writer.Pod(static_cast<std::uint32_t>(v.GetCount()));
				for (std::size_t i = 0; i < v.GetCount(); ++i)
				{
					const Object* elem = v.Get(i);
					const Class* klass = elem != nullptr ? elem->GetDynamicClass() : nullptr;
					const std::string className = klass != nullptr ? klass->GetFullyQualifiedName() : std::string{};
					writer.Name(className);
					if (elem != nullptr)
					{
						WriteObject(writer, *elem, emitClass);
					}
					else
					{
						writer.TagByte(Tag::End);
					}
				}
			}
			void Visit(std::string_view name, refl::Object& v) override
			{
				writer.TagByte(Tag::Object);
				writer.Name(name);
				WriteObject(writer, v, emitClass);
			}
		};

		void WriteObject(Writer& writer, const Object& obj, bool emitClass)
		{
			const Class* klass = obj.GetDynamicClass();
			if (emitClass && klass != nullptr && klass->GetName() != "Object")
			{
				writer.TagByte(Tag::String);
				writer.Name(kClassKey);
				writer.Text(klass->GetFullyQualifiedName());
			}

			writer.TagByte(Tag::String);
			writer.Name(kUuidKey);
			writer.Text(obj.GetUuid().ToString());

			BinarySerializer serializer{writer, emitClass};
			const_cast<Object&>(obj).Accept(serializer);
			writer.TagByte(Tag::End);
		}

		class Reader
		{
		public:
			explicit Reader(std::span<const std::byte> data) :
				m_data(data)
			{
			}

			[[nodiscard]] bool Ok() const
			{
				return m_ok;
			}
			[[nodiscard]] std::size_t Pos() const
			{
				return m_pos;
			}
			void Seek(std::size_t pos)
			{
				m_pos = pos;
			}

			bool Raw(void* dest, std::size_t size)
			{
				if (!m_ok || m_pos + size > m_data.size())
				{
					m_ok = false;
					return false;
				}
				std::memcpy(dest, m_data.data() + m_pos, size);
				m_pos += size;
				return true;
			}

			template<typename T>
			T Pod()
			{
				T value{};
				Raw(&value, sizeof(T));
				return value;
			}

			Tag TagByte()
			{
				return static_cast<Tag>(Pod<std::uint8_t>());
			}

			std::string Name()
			{
				const auto size = Pod<std::uint16_t>();
				return Bytes(size);
			}

			std::string Text()
			{
				const auto size = Pod<std::uint32_t>();
				return Bytes(size);
			}

		private:
			std::string Bytes(std::size_t size)
			{
				if (!m_ok || m_pos + size > m_data.size())
				{
					m_ok = false;
					return {};
				}
				std::string out(reinterpret_cast<const char*>(m_data.data() + m_pos), size);
				m_pos += size;
				return out;
			}

			std::span<const std::byte> m_data;
			std::size_t m_pos = 0;
			bool m_ok = true;
		};

		void SkipValue(Reader& reader, Tag tag);

		void SkipObject(Reader& reader)
		{
			while (reader.Ok())
			{
				const Tag tag = reader.TagByte();
				if (tag == Tag::End || !reader.Ok())
					return;
				reader.Name();
				SkipValue(reader, tag);
			}
		}

		void SkipValue(Reader& reader, Tag tag)
		{
			switch (tag)
			{
				case Tag::NativeInt:
				case Tag::Int32:
				case Tag::UInt32:
				case Tag::Float32:
				case Tag::NativeFloat:
					reader.Pod<std::uint32_t>();
					return;
				case Tag::NativeBool:
				case Tag::Boolean:
				case Tag::Int8:
				case Tag::UInt8:
					reader.Pod<std::uint8_t>();
					return;
				case Tag::Int16:
				case Tag::UInt16:
					reader.Pod<std::uint16_t>();
					return;
				case Tag::Int64:
				case Tag::UInt64:
				case Tag::Float64:
				case Tag::Enumeration:
					reader.Pod<std::uint64_t>();
					return;
				case Tag::NativeString:
				case Tag::String:
				case Tag::File:
				case Tag::Folder:
					reader.Text();
					return;
				case Tag::Object:
					SkipObject(reader);
					return;
				case Tag::Vector:
				{
					const auto count = reader.Pod<std::uint32_t>();
					for (std::uint32_t i = 0; i < count && reader.Ok(); ++i)
					{
						reader.Name();
						SkipObject(reader);
					}
					return;
				}
				default:
					return;
			}
		}

		struct FieldRef
		{
			Tag tag = Tag::End;
			std::size_t pos = 0;
		};

		using FieldMap = std::map<std::string, FieldRef, std::less<>>;

		FieldMap ReadLevel(Reader& reader)
		{
			FieldMap fields;
			while (reader.Ok())
			{
				const Tag tag = reader.TagByte();
				if (tag == Tag::End || !reader.Ok())
					break;
				const std::string name = reader.Name();
				if (!reader.Ok())
					break;
				fields.emplace(name, FieldRef{tag, reader.Pos()});
				SkipValue(reader, tag);
			}
			return fields;
		}

		void ApplyUuid(Object& target, const FieldMap& fields, Reader& reader)
		{
			const auto it = fields.find(kUuidKey);
			if (it == fields.end() || it->second.tag != Tag::String)
				return;
			reader.Seek(it->second.pos);
			const std::string text = reader.Text();
			if (!text.empty())
				target.SetUuid(cct::Uuid::FromString(text));
		}

		struct BinaryDeserializer : FieldVisitor
		{
			Reader& reader;
			FieldMap fields;
			Registry* registry;

			BinaryDeserializer(Reader& r, FieldMap f, Registry* reg) :
				reader(r),
				fields(std::move(f)),
				registry(reg)
			{
			}

			const FieldRef* Find(std::string_view name, Tag expected) const
			{
				const auto it = fields.find(name);
				if (it == fields.end() || it->second.tag != expected)
					return nullptr;
				return &it->second;
			}

			template<typename T>
			bool Scalar(std::string_view name, Tag tag, T& out)
			{
				const FieldRef* ref = Find(name, tag);
				if (ref == nullptr)
					return false;
				reader.Seek(ref->pos);
				out = reader.Pod<T>();
				return true;
			}

			bool Textual(std::string_view name, Tag tag, std::string& out)
			{
				const FieldRef* ref = Find(name, tag);
				if (ref == nullptr)
					return false;
				reader.Seek(ref->pos);
				out = reader.Text();
				return true;
			}

			void Visit(std::string_view name, int& v) override
			{
				std::int32_t tmp = 0;
				if (Scalar(name, Tag::NativeInt, tmp))
					v = tmp;
			}
			void Visit(std::string_view name, float& v) override
			{
				float tmp = 0.0F;
				if (Scalar(name, Tag::NativeFloat, tmp))
					v = tmp;
			}
			void Visit(std::string_view name, bool& v) override
			{
				std::uint8_t tmp = 0;
				if (Scalar(name, Tag::NativeBool, tmp))
					v = tmp != 0;
			}
			void Visit(std::string_view name, std::string& v) override
			{
				std::string tmp;
				if (Textual(name, Tag::NativeString, tmp))
					v = std::move(tmp);
			}
			void Visit(std::string_view name, refl::Boolean& v) override
			{
				std::uint8_t tmp = 0;
				if (Scalar(name, Tag::Boolean, tmp))
					v.Set(tmp != 0);
			}
			void Visit(std::string_view name, refl::Int8& v) override
			{
				cct::Int8 tmp = 0;
				if (Scalar(name, Tag::Int8, tmp))
					v.Set(tmp);
			}
			void Visit(std::string_view name, refl::Int16& v) override
			{
				cct::Int16 tmp = 0;
				if (Scalar(name, Tag::Int16, tmp))
					v.Set(tmp);
			}
			void Visit(std::string_view name, refl::Int32& v) override
			{
				cct::Int32 tmp = 0;
				if (Scalar(name, Tag::Int32, tmp))
					v.Set(tmp);
			}
			void Visit(std::string_view name, refl::Int64& v) override
			{
				cct::Int64 tmp = 0;
				if (Scalar(name, Tag::Int64, tmp))
					v.Set(tmp);
			}
			void Visit(std::string_view name, refl::UInt8& v) override
			{
				cct::UInt8 tmp = 0;
				if (Scalar(name, Tag::UInt8, tmp))
					v.Set(tmp);
			}
			void Visit(std::string_view name, refl::UInt16& v) override
			{
				cct::UInt16 tmp = 0;
				if (Scalar(name, Tag::UInt16, tmp))
					v.Set(tmp);
			}
			void Visit(std::string_view name, refl::UInt32& v) override
			{
				cct::UInt32 tmp = 0;
				if (Scalar(name, Tag::UInt32, tmp))
					v.Set(tmp);
			}
			void Visit(std::string_view name, refl::UInt64& v) override
			{
				cct::UInt64 tmp = 0;
				if (Scalar(name, Tag::UInt64, tmp))
					v.Set(tmp);
			}
			void Visit(std::string_view name, refl::Float32& v) override
			{
				cct::Float32 tmp = 0.0F;
				if (Scalar(name, Tag::Float32, tmp))
					v.Set(tmp);
			}
			void Visit(std::string_view name, refl::Float64& v) override
			{
				cct::Float64 tmp = 0.0;
				if (Scalar(name, Tag::Float64, tmp))
					v.Set(tmp);
			}
			void Visit(std::string_view name, refl::String& v) override
			{
				std::string tmp;
				if (Textual(name, Tag::String, tmp))
					v.Set(std::string_view(tmp));
			}
			void Visit(std::string_view name, refl::File& v) override
			{
				std::string tmp;
				if (Textual(name, Tag::File, tmp))
					v.Set(std::string_view(tmp));
			}
			void Visit(std::string_view name, refl::Folder& v) override
			{
				std::string tmp;
				if (Textual(name, Tag::Folder, tmp))
					v.Set(std::string_view(tmp));
			}
			void Visit(std::string_view name, refl::Enumeration& v) override
			{
				std::int64_t tmp = 0;
				if (Scalar(name, Tag::Enumeration, tmp))
					v.SetEnumValue(tmp);
			}
			void Visit(std::string_view name, refl::Object& v) override
			{
				const FieldRef* ref = Find(name, Tag::Object);
				if (ref == nullptr)
					return;
				reader.Seek(ref->pos);
				FieldMap nested = ReadLevel(reader);
				ApplyUuid(v, nested, reader);
				BinaryDeserializer nestedDeser{reader, std::move(nested), registry};
				v.Accept(nestedDeser);
			}
			void Visit(std::string_view name, refl::Vector& v) override
			{
				const FieldRef* ref = Find(name, Tag::Vector);
				if (ref == nullptr)
					return;

				reader.Seek(ref->pos);
				const auto count = reader.Pod<std::uint32_t>();

				v.Clear();
				ScopedObjectFlag constructing(v, ObjectFlags::Constructing,
											  [&v]()
											  { v.OnValueChanged.Emit(); });

				for (std::uint32_t i = 0; i < count && reader.Ok(); ++i)
				{
					const std::string className = reader.Name();
					const std::size_t elemPos = reader.Pos();

					const Class* cls = v.GetElementType();
					if (!className.empty())
					{
						if (const Class* named = GetClassByName(className))
							cls = named;
					}

					reader.Seek(elemPos);
					FieldMap elemFields = ReadLevel(reader);
					const std::size_t afterElem = reader.Pos();

					if (cls == nullptr)
					{
						reader.Seek(afterElem);
						continue;
					}

					auto newObj = registry != nullptr ? registry->Allocate(cls) : cls->CreateDefaultObject();
					if (!newObj)
					{
						reader.Seek(afterElem);
						continue;
					}

					Object* elem = newObj.get();
					ApplyUuid(*elem, elemFields, reader);
					v.Add(std::move(newObj));

					BinaryDeserializer elemDeser{reader, std::move(elemFields), registry};
					elem->Accept(elemDeser);
					reader.Seek(afterElem);
				}
			}
		};

	} // namespace

	std::vector<std::byte> Binary::ToBinary(const Object& obj, bool emitClass)
	{
		std::vector<std::byte> out;
		Writer writer{out};
		WriteObject(writer, obj, emitClass);
		return out;
	}

	bool Binary::FromBinary(Object& target, std::span<const std::byte> data, Registry* registry)
	{
		Reader reader{data};
		FieldMap fields = ReadLevel(reader);
		if (!reader.Ok())
			return false;

		ApplyUuid(target, fields, reader);
		BinaryDeserializer deser{reader, std::move(fields), registry};
		target.Accept(deser);
		return reader.Ok();
	}
} // namespace cct::refl
