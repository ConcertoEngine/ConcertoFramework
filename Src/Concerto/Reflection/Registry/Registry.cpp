//
// Created by arthur on 20/12/2024.
//

#include "Concerto/Reflection/Registry/Registry.hpp"

#include <algorithm>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	Registry::Registry(std::size_t defaultObjectCount) :
		m_defaultObjectCount(defaultObjectCount)

	{
	}

	std::unique_ptr<Object> Registry::Allocate(const Class* klass)
	{
		if (klass == nullptr)
		{
			CCT_ASSERT_FALSE("Registry::Allocate: klass is nullptr");
			return nullptr;
		}

		// klass->CreateDefaultObject() alone leaves the object untracked; Track() is what
		// actually puts it in this catalog.
		auto object = klass->CreateDefaultObject();
		if (object == nullptr)
			return nullptr;

		Track(*object);
		return object;
	}

	void Registry::Track(Object& object)
	{
		TrackInternal(object, nullptr);
	}

	Handle Registry::Adopt(std::unique_ptr<Object> object)
	{
		if (!object)
			return {};

		Object& ref = *object;
		return TrackInternal(ref, std::move(object));
	}

	Handle Registry::TrackInternal(Object& object, std::unique_ptr<Object> owned)
	{
		if (object.GetRegistry() == this)
		{
			const Handle existing = object.GetHandle();
			if (owned != nullptr && !existing.IsNull() && existing.m_index < m_slots.size())
				m_slots[existing.m_index].m_owned = std::move(owned);
			return existing;
		}

		// Re-homing: remove the stale entry from wherever it's currently tracked first,
		// or that registry is left holding a dangling pointer once `object` is destroyed.
		if (Registry* current = object.GetRegistry())
			current->Untrack(object);

		const Class* klass = object.GetDynamicClass();
		if (klass == nullptr)
		{
			CCT_ASSERT_FALSE("Cannot track an object with no dynamic class");
			return {};
		}

		const Handle handle = AcquireSlot(object, std::move(owned));
		object.SetRegistry(this);
		object.SetHandle(handle);
		IndexObject(klass, object, handle);
		m_byUuid.emplace(object.GetUuid().ToString(), &object);
		return handle;
	}

	void Registry::IndexObject(const Class* klass, Object& object, Handle handle)
	{
		auto it = m_objects.find(klass);
		if (it == m_objects.end())
		{
			it = m_objects.emplace(klass, std::vector<Object*>{}).first;
			it->second.reserve(m_defaultObjectCount);
		}

		if (handle.m_index < m_slots.size())
		{
			m_slots[handle.m_index].m_bucketIndex = static_cast<std::uint32_t>(it->second.size());
		}
		it->second.push_back(&object);
	}

	void Registry::UnindexObject(Object& object)
	{
		m_byUuid.erase(object.GetUuid().ToString());

		const Class* klass = object.GetDynamicClass();
		if (klass == nullptr)
			return;

		auto it = m_objects.find(klass);
		if (it == m_objects.end())
			return;

		const Handle handle = object.GetHandle();
		if (handle.IsNull() || handle.m_index >= m_slots.size())
			return;
		if (m_slots[handle.m_index].m_generation != handle.m_generation)
			return;

		auto& bucket = it->second;
		const std::uint32_t index = m_slots[handle.m_index].m_bucketIndex;
		if (index >= bucket.size() || bucket[index] != &object)
			return;

		const std::size_t last = bucket.size() - 1;
		if (index != last)
		{
			Object* moved = bucket[last];
			bucket[index] = moved;
			const Handle movedHandle = moved->GetHandle();
			if (!movedHandle.IsNull() && movedHandle.m_index < m_slots.size())
			{
				m_slots[movedHandle.m_index].m_bucketIndex = index;
			}
		}
		bucket.pop_back();
	}

	Handle Registry::AcquireSlot(Object& object, std::unique_ptr<Object> owned)
	{
		if (!m_freeSlots.empty())
		{
			const std::uint32_t index = m_freeSlots.back();
			m_freeSlots.pop_back();
			Slot& slot = m_slots[index];
			slot.m_object = &object;
			slot.m_owned = std::move(owned);
			return Handle{index, slot.m_generation};
		}

		const auto index = static_cast<std::uint32_t>(m_slots.size());
		m_slots.push_back(Slot{&object, std::move(owned), 1});
		return Handle{index, 1};
	}

	void Registry::ReleaseSlot(Handle handle)
	{
		if (handle.IsNull() || handle.m_index >= m_slots.size())
			return;

		Slot& slot = m_slots[handle.m_index];
		if (slot.m_generation != handle.m_generation)
			return;

		slot.m_object = nullptr;
		++slot.m_generation;
		if (slot.m_generation != 0)
			m_freeSlots.push_back(handle.m_index);
	}

	Object* Registry::Resolve(Handle handle) const
	{
		if (handle.IsNull() || handle.m_index >= m_slots.size())
			return nullptr;

		const Slot& slot = m_slots[handle.m_index];
		if (slot.m_generation != handle.m_generation)
			return nullptr;

		return slot.m_object;
	}

	bool Registry::IsValid(Handle handle) const
	{
		return Resolve(handle) != nullptr;
	}

	void Registry::Untrack(Object& object)
	{
		if (object.GetRegistry() != this)
			return;

		if (Owns(object.GetHandle()))
		{
			CCT_ASSERT_FALSE("Untrack() on an owned object — use Release() or Destroy()");
			return;
		}

		const Handle handle = object.GetHandle();
		UnindexObject(object);
		ReleaseSlot(handle);
		object.SetHandle({});
		object.SetRegistry(nullptr);
	}

	void Registry::RekeyUuid(Object& object, const std::string& previousKey)
	{
		const auto it = m_byUuid.find(previousKey);
		if (it != m_byUuid.end() && it->second == &object)
		{
			m_byUuid.erase(it);
		}
		m_byUuid.emplace(object.GetUuid().ToString(), &object);
	}

	Object* Registry::FindByUuid(std::string_view uuid) const
	{
		const auto it = m_byUuid.find(std::string(uuid));
		return it != m_byUuid.end() ? it->second : nullptr;
	}

	std::unique_ptr<Object> Registry::Release(Handle handle)
	{
		if (handle.IsNull() || handle.m_index >= m_slots.size())
			return nullptr;

		Slot& slot = m_slots[handle.m_index];
		if (slot.m_generation != handle.m_generation || slot.m_owned == nullptr)
			return nullptr;

		std::unique_ptr<Object> owned = std::move(slot.m_owned);
		UnindexObject(*owned);
		owned->SetHandle({});
		owned->SetRegistry(nullptr);
		ReleaseSlot(handle);
		return owned;
	}

	void Registry::Destroy(Handle handle)
	{
		if (std::unique_ptr<Object> owned = Release(handle))
		{
			m_pendingDestruction.push_back(std::move(owned));
		}
	}

	void Registry::CollectGarbage()
	{
		while (!m_pendingDestruction.empty())
		{
			std::vector<std::unique_ptr<Object>> batch;
			batch.swap(m_pendingDestruction);
			batch.clear();
		}
	}

	bool Registry::Owns(Handle handle) const
	{
		if (handle.IsNull() || handle.m_index >= m_slots.size())
			return false;

		const Slot& slot = m_slots[handle.m_index];
		return slot.m_generation == handle.m_generation && slot.m_owned != nullptr;
	}

	std::size_t Registry::GetPendingDestructionCount() const
	{
		return m_pendingDestruction.size();
	}

	void Registry::DetachAll()
	{
		for (Slot& slot : m_slots)
		{
			if (slot.m_object != nullptr && slot.m_owned == nullptr)
			{
				slot.m_object->SetHandle({});
				slot.m_object->SetRegistry(nullptr);
			}
			else if (slot.m_owned != nullptr)
			{
				slot.m_owned->SetHandle({});
				slot.m_owned->SetRegistry(nullptr);
			}
		}
	}

	Registry::~Registry()
	{
		for (std::unique_ptr<Object>& pending : m_pendingDestruction)
		{
			if (pending != nullptr)
			{
				pending->SetRegistry(nullptr);
			}
		}

		DetachAll();
		m_pendingDestruction.clear();
		m_byUuid.clear();
		m_objects.clear();
		m_slots.clear();
	}

	Registry::Registry(Registry&& other) noexcept :
		m_defaultObjectCount(other.m_defaultObjectCount),
		m_objects(std::move(other.m_objects)),
		m_slots(std::move(other.m_slots)),
		m_freeSlots(std::move(other.m_freeSlots)),
		m_pendingDestruction(std::move(other.m_pendingDestruction)),
		m_byUuid(std::move(other.m_byUuid))
	{
		Rehome();
		other.m_objects.clear();
		other.m_slots.clear();
		other.m_freeSlots.clear();
	}

	Registry& Registry::operator=(Registry&& other) noexcept
	{
		if (this == &other)
			return *this;

		DetachAll();
		m_pendingDestruction.clear();

		m_defaultObjectCount = other.m_defaultObjectCount;
		m_objects = std::move(other.m_objects);
		m_slots = std::move(other.m_slots);
		m_freeSlots = std::move(other.m_freeSlots);
		m_pendingDestruction = std::move(other.m_pendingDestruction);
		m_byUuid = std::move(other.m_byUuid);

		Rehome();
		other.m_objects.clear();
		other.m_slots.clear();
		other.m_freeSlots.clear();
		return *this;
	}

	void Registry::Rehome()
	{
		for (Slot& slot : m_slots)
		{
			Object* object = slot.m_owned != nullptr ? slot.m_owned.get() : slot.m_object;
			if (object != nullptr)
			{
				object->SetRegistry(this);
			}
		}
	}

	std::size_t Registry::GetObjectCount() const
	{
		std::size_t count = 0;
		for (const auto& [klass, objects] : m_objects)
			count += objects.size();
		return count;
	}

	std::size_t Registry::GetObjectCount(const Class* klass) const
	{
		auto it = m_objects.find(klass);
		return it != m_objects.end() ? it->second.size() : 0;
	}

	std::span<Object* const> Registry::GetObjects(const Class* klass) const
	{
		auto it = m_objects.find(klass);
		if (it == m_objects.end())
			return {};
		return it->second;
	}

	void Registry::ForEachObject(const std::function<void(Object&)>& visitor) const
	{
		if (!visitor)
			return;
		for (const auto& [klass, objects] : m_objects)
			for (Object* object : objects)
				if (object != nullptr)
					visitor(*object);
	}
} // namespace cct::refl
