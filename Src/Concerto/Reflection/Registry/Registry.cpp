//
// Created by arthur on 20/12/2024.
//

#include "Concerto/Reflection/Registry/Registry.hpp"

#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	namespace
	{
		void NotifyChange(Object& changedObject, EnumFlags<ChangeType> changeType, std::string_view variableName)
		{
			using namespace std::string_view_literals;

			const Class* klass = changedObject.GetDynamicClass();
			if (klass == nullptr)
			{
				CCT_ASSERT_FALSE("Could not found dynamic class from object");
				return;
			}

			for (std::size_t i = 0; i < klass->GetMethodCount(); ++i)
			{
				const Method* method = klass->GetMethod(i);
				if (method == nullptr) [[unlikely]]
					continue;
				if (method->GetAttribute("ChangedEvent") != variableName)
					continue;
				Result<void, std::string> result = method->Invoke<void>(changedObject, changedObject, changeType);
				if (result.IsError())
					cct::Logger::Error("'{}' method returned an error: {}", method->GetName(), result.GetError());
				return;
			}
		}
	} // namespace

	Registry::Registry(std::size_t defaultObjectCount) :
		m_defaultObjectCount(defaultObjectCount)

	{
	}

	void Registry::NotifyPreChange(Object& changedObject, ChangeType changeType, std::string_view variableName)
	{
		NotifyChange(changedObject, changeType | ChangeType::PreChange, variableName);
	}

	void Registry::NotifyPostChange(Object& changedObject, ChangeType changeType, std::string_view variableName)
	{
		NotifyChange(changedObject, changeType | ChangeType::PostChange, variableName);
	}

	Object* Registry::Allocate(const Class* klass)
	{
		auto it = m_objects.find(klass);
		if (it == m_objects.end())
		{
			it = m_objects.emplace(klass, std::vector<std::unique_ptr<Object>>{}).first;
			it->second.reserve(m_defaultObjectCount);
		}

		auto object = klass->CreateDefaultObject();
		if (object == nullptr)
			return nullptr;

		return it->second.emplace_back(std::move(object)).get();
	}

	void Registry::DeAllocate(const Object& object)
	{
		const Class* klass = object.GetDynamicClass();
		if (klass == nullptr)
		{
			CCT_ASSERT_FALSE("Invalid object dynamic class");
			return;
		}
		auto it = m_objects.find(klass);
		if (it == m_objects.end())
		{
			CCT_ASSERT_FALSE("Trying to deallocate '{}', but the Registry doesn't own it", klass->GetName());
			return;
		}

		auto objectIt = std::ranges::find_if(it->second, [&](std::unique_ptr<Object>& obj)
											 { return obj.get() == &object; });

		if (objectIt == it->second.end())
		{
			CCT_ASSERT_FALSE("Trying to deallocate '{}', but the Registry doesn't own it", klass->GetName());
			return;
		}

		it->second.erase(objectIt);
	}
} // namespace cct::refl
