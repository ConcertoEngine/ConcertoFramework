//
// Created by arthur on 17/08/2022.
//

#include "Concerto/Graphics/Core/Input/Input.hpp"

#include <chrono>

namespace cct
{
	Connection Input::Register(const std::string& name, Key key, TriggerType triggerType, std::function<void()> callback)
	{
		auto [it, inserted] = m_keyBindings.try_emplace(name);
		it->second.first.Emplace(static_cast<std::size_t>(key), true);
		return it->second.second[triggerType].Connect(std::move(callback));
	}

	Connection Input::Register(const std::string& name, MouseEvent::Type key, MouseEventCallback callback)
	{
		auto [it, inserted] = m_mouseBindings.try_emplace(name, MouseBinding{key, {}});
		return it->second.second.Connect(std::move(callback));
	}

	Connection Input::Register(const std::string& name, Key key, TriggerType triggerType, const Trackable& context, std::function<void()> callback)
	{
		auto [it, inserted] = m_keyBindings.try_emplace(name);
		it->second.first.Emplace(static_cast<std::size_t>(key), true);
		return it->second.second[triggerType].Connect(context, std::move(callback));
	}

	Connection Input::Register(const std::string& name, MouseEvent::Type key, const Trackable& context, MouseEventCallback callback)
	{
		auto [it, inserted] = m_mouseBindings.try_emplace(name, MouseBinding{key, {}});
		return it->second.second.Connect(context, std::move(callback));
	}

	void Input::Trigger(const std::vector<Event>& events)
	{
		for (const auto& [type, name, data] : events)
		{
			if (type == Event::Type::Key)
				TriggerKeyEvent(std::get<KeyEvent>(data));
			else if (type == Event::Type::Mouse)
				TriggerMouseEvent(std::get<MouseEvent>(data));
		}
	}

	void Input::TriggerKeyEvent(const KeyEvent& keyEvent)
	{
		for (auto& [name, binding] : m_keyBindings)
		{
			const auto keyIndex = static_cast<std::size_t>(keyEvent.key);
			if (!binding.first.Has(keyIndex) || !binding.first[keyIndex])
				continue;
			auto it = binding.second.find(keyEvent.triggerType);
			if (it == binding.second.end())
				continue;
			it->second.Emit();
		}
	}

	void Input::TriggerMouseEvent(const MouseEvent& mouseEvent)
	{
		for (auto& [name, binding] : m_mouseBindings)
		{
			if (binding.first != mouseEvent.type)
				continue;
			binding.second.Emit(mouseEvent);
		}
	}
} // namespace cct
