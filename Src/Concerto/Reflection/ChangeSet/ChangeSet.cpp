//
// Created by arthur on 18/08/2026.
//

#include "Concerto/Reflection/ChangeSet/ChangeSet.hpp"

#include <charconv>
#include <array>

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
		template<typename T>
		std::string Exact(T value)
		{
			std::array<char, 64> buffer{};
			const auto res = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
			if (res.ec != std::errc{})
				return std::to_string(value);
			return std::string(buffer.data(), res.ptr);
		}

		struct Walker : FieldVisitor
		{
			ChangeSet& out;
			std::string ownerUuid;
			std::string prefix;
			bool clearOnly;

			Walker(ChangeSet& o, std::string uuid, std::string p, bool clearOnly_) :
				out(o),
				ownerUuid(std::move(uuid)),
				prefix(std::move(p)),
				clearOnly(clearOnly_)
			{
			}

			[[nodiscard]] std::string PathFor(std::string_view name) const
			{
				return prefix.empty() ? std::string(name) : prefix + std::string(name);
			}

			void Record(std::string_view name, Object& field, std::string value)
			{
				if (!field.HasFlag(ObjectFlags::Dirty))
					return;
				field.ClearFlag(ObjectFlags::Dirty);
				if (clearOnly)
					return;
				out.m_changes.push_back(FieldChange{ownerUuid, PathFor(name), std::move(value)});
			}

			void Visit(std::string_view name, refl::Boolean& v) override
			{
				Record(name, v, v.Get() ? "true" : "false");
			}
			void Visit(std::string_view name, refl::Int8& v) override
			{
				Record(name, v, std::to_string(static_cast<int>(v.Get())));
			}
			void Visit(std::string_view name, refl::Int16& v) override
			{
				Record(name, v, std::to_string(v.Get()));
			}
			void Visit(std::string_view name, refl::Int32& v) override
			{
				Record(name, v, std::to_string(v.Get()));
			}
			void Visit(std::string_view name, refl::Int64& v) override
			{
				Record(name, v, std::to_string(v.Get()));
			}
			void Visit(std::string_view name, refl::UInt8& v) override
			{
				Record(name, v, std::to_string(static_cast<unsigned>(v.Get())));
			}
			void Visit(std::string_view name, refl::UInt16& v) override
			{
				Record(name, v, std::to_string(v.Get()));
			}
			void Visit(std::string_view name, refl::UInt32& v) override
			{
				Record(name, v, std::to_string(v.Get()));
			}
			void Visit(std::string_view name, refl::UInt64& v) override
			{
				Record(name, v, std::to_string(v.Get()));
			}
			void Visit(std::string_view name, refl::Float32& v) override
			{
				Record(name, v, Exact(v.Get()));
			}
			void Visit(std::string_view name, refl::Float64& v) override
			{
				Record(name, v, Exact(v.Get()));
			}
			void Visit(std::string_view name, refl::String& v) override
			{
				Record(name, v, v.Get());
			}
			void Visit(std::string_view name, refl::File& v) override
			{
				Record(name, v, v.Get());
			}
			void Visit(std::string_view name, refl::Folder& v) override
			{
				Record(name, v, v.Get());
			}
			void Visit(std::string_view name, refl::Enumeration& v) override
			{
				Record(name, v, v.ToString());
			}
			void Visit(std::string_view name, refl::Object& v) override
			{
				Walker nested{out, ownerUuid, PathFor(name) + ".", clearOnly};
				v.Accept(nested);
			}
			void Visit(std::string_view name, refl::Vector& v) override
			{
				if (v.HasFlag(ObjectFlags::Dirty))
				{
					v.ClearFlag(ObjectFlags::Dirty);
					if (!clearOnly)
						out.m_structural = true;
				}

				for (std::size_t i = 0; i < v.GetCount(); ++i)
				{
					Object* elem = v.Get(i);
					if (elem == nullptr)
						continue;
					Walker elemWalker{out, elem->GetUuid().ToString(), std::string{}, clearOnly};
					elem->Accept(elemWalker);
				}
			}
		};

		struct Finder : FieldVisitor
		{
			std::string_view wanted;
			Object*& result;

			Finder(std::string_view w, Object*& r) :
				wanted(w),
				result(r)
			{
			}

			void Visit(std::string_view, refl::Object& v) override
			{
				if (result != nullptr)
					return;
				if (v.GetUuid().ToString() == wanted)
				{
					result = &v;
					return;
				}
				Finder nested{wanted, result};
				v.Accept(nested);
			}

			void Visit(std::string_view, refl::Vector& v) override
			{
				for (std::size_t i = 0; i < v.GetCount() && result == nullptr; ++i)
				{
					Object* elem = v.Get(i);
					if (elem == nullptr)
						continue;
					if (elem->GetUuid().ToString() == wanted)
					{
						result = elem;
						return;
					}
					Finder nested{wanted, result};
					elem->Accept(nested);
				}
			}
		};

		void Run(Object& root, ChangeSet& out, bool clearOnly)
		{
			Walker walker{out, root.GetUuid().ToString(), std::string{}, clearOnly};
			root.Accept(walker);
		}
	} // namespace

	Object* ChangeCollector::FindByUuid(Object& root, std::string_view uuid)
	{
		if (root.GetUuid().ToString() == uuid)
			return &root;

		Object* found = nullptr;
		Finder finder{uuid, found};
		root.Accept(finder);
		return found;
	}

	ChangeSet ChangeCollector::Collect(Object& root)
	{
		ChangeSet set;
		Run(root, set, false);
		return set;
	}

	void ChangeCollector::ClearDirty(Object& root)
	{
		ChangeSet discarded;
		Run(root, discarded, true);
	}
} // namespace cct::refl
