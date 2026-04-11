//
// Created by arthur on 17/08/2022.
//

#include <chrono>

#include "Concerto/Graphics/Core/Input/Input.hpp"

namespace cct
{
	void Input::Register(const std::string& name, Key key, TriggerType triggerType, std::function<void()>&& callback)
	{
		auto it = m_keyCallbacks.find(name);
		if (it == m_keyCallbacks.end())
		{
			auto pair = std::make_pair(SparseVector<bool>(), TriggerTypeCallbacks());
			pair.first.Emplace(static_cast<std::size_t>(key), true);
			pair.second[triggerType].push_back(callback);
			m_keyCallbacks.emplace(name, std::move(pair));
			return;
		}
		it->second.first.Emplace(static_cast<std::size_t>(key), true);
		it->second.second[triggerType].push_back(callback);
	}

	void Input::Register(const std::string& name, MouseEvent::Type key, MouseEventCallback&& callback)
	{
		const auto it = m_mouseCallback.find(name);
		if (it == m_mouseCallback.end())
		{
			std::vector<MouseEventCallback> vec = { callback };
			auto pair = std::make_pair(key, std::move(vec));
			m_mouseCallback.emplace(name, std::move(pair));
			return;
		}
		it->second.second.push_back(callback);
	}

	void Input::Trigger(const std::vector<Event>& events)
	{
		for (const auto& [type, name, data]: events)
		{
			if (type == Event::Type::Key)
				TriggerKeyEvent(std::get<KeyEvent>(data));
			else if (type == Event::Type::Mouse)
				TriggerMouseEvent(std::get<MouseEvent>(data));
		}
	}

	void Input::TriggerKeyEvent(const KeyEvent& keyEvent)
	{
		for (auto& [key, bindingCallback]: m_keyCallbacks)
		{
			const auto keyIndex = static_cast<std::size_t>(keyEvent.key);
			if (!bindingCallback.first.Has(keyIndex) || !bindingCallback.first[keyIndex])
				continue;
			auto it = bindingCallback.second.find(keyEvent.triggerType);
			if (it == bindingCallback.second.end())
				continue;
			for (auto& callback: it->second)
				callback();
		}
	}

	void Input::TriggerMouseEvent(const MouseEvent& mouseEvent)
	{
		for (const auto& [key, callbacks]: m_mouseCallback)
		{
			if (callbacks.first != mouseEvent.type)
				continue;
			for (auto& callback: callbacks.second)
			{
				if (callback)
					callback(mouseEvent);
			}
		}
	}
}
