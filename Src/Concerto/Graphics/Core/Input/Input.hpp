//
// Created by arthur on 17/08/2022.
//

#ifndef CONCERTO_INPUT_HPP
#define CONCERTO_INPUT_HPP

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <Concerto/Core/Signal/Signal.hpp>
#include <Concerto/Core/Signal/Trackable.hpp>
#include <Concerto/Core/SparseVector/SparseVector.hpp>

#include "Concerto/Graphics/Core/Defines.hpp"
#include "Concerto/Graphics/Core/Window/Event.hpp"
#include "Concerto/Graphics/Core/Window/Key.hpp"

namespace cct
{
	class CONCERTO_GRAPHICS_CORE_API Input
	{
	public:
		using TriggerTypeSignals = std::unordered_map<TriggerType, Signal<>>;
		using KeyBinding = std::pair<SparseVector<bool>, TriggerTypeSignals>;
		using MouseEventCallback = std::function<void(const MouseEvent&)>;
		using MouseBinding = std::pair<MouseEvent::Type, Signal<const MouseEvent&>>;

		Input() = default;
		~Input() = default;
		Input(const Input&) = delete;
		Input(Input&&) = default;

		Input& operator=(const Input&) = delete;
		Input& operator=(Input&&) = default;

		[[nodiscard]] Connection Register(const std::string& name, Key key, TriggerType triggerType, std::function<void()> callback);
		[[nodiscard]] Connection Register(const std::string& name, MouseEvent::Type key, MouseEventCallback callback);

		Connection Register(const std::string& name, Key key, TriggerType triggerType, const Trackable& context, std::function<void()> callback);
		Connection Register(const std::string& name, MouseEvent::Type key, const Trackable& context, MouseEventCallback callback);

		void Trigger(const std::vector<Event>& events);
		void TriggerKeyEvent(const KeyEvent& keyEvent);
		void TriggerMouseEvent(const MouseEvent& mouseEvent);

	private:
		std::unordered_map<std::string, KeyBinding> m_keyBindings;
		std::unordered_map<std::string, MouseBinding> m_mouseBindings;
	};
} // namespace cct
#endif // CONCERTO_INPUT_HPP
