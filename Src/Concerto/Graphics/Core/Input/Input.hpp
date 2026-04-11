//
// Created by arthur on 17/08/2022.
//

#ifndef CONCERTO_INPUT_HPP
#define CONCERTO_INPUT_HPP

#include <functional>
#include <unordered_map>
#include <string>
#include <vector>

#include <Concerto/Core/FunctionRef/FunctionRef.hpp>
#include <Concerto/Core/SparseVector/SparseVector.hpp>

#include "Concerto/Graphics/Core/Defines.hpp"
#include "Concerto/Graphics/Core/Window/Event.hpp"
#include "Concerto/Graphics/Core/Window/Key.hpp"

namespace cct
{
	class CONCERTO_GRAPHICS_CORE_API Input
	{
	public:
		using KeyCallbacks = std::vector<std::function<void()>>;
		using TriggerTypeCallbacks = std::unordered_map<TriggerType, KeyCallbacks>;
		using BindingCallback = std::pair<SparseVector<bool>, TriggerTypeCallbacks>;
		using MouseEventCallback = std::function<void(const MouseEvent&)>;

		Input() = default;
		~Input() = default;
		Input(const Input&) = delete;
		Input(Input&&) = default;

		Input& operator=(const Input&) = delete;
		Input& operator=(Input&&) = default;


		void Register(const std::string& name, Key key, TriggerType triggerType, std::function<void()>&& callback);
		void Register(const std::string& name, MouseEvent::Type key, MouseEventCallback&& callback);

		void Trigger(const std::vector<Event>& events);
		void TriggerKeyEvent(const KeyEvent& keyEvent);
		void TriggerMouseEvent(const MouseEvent& mouseEvent);

	private:
		std::unordered_map<std::string, BindingCallback> m_keyCallbacks;
		std::unordered_map<std::string, std::pair<MouseEvent::Type, std::vector<MouseEventCallback>>> m_mouseCallback;
	};
}
#endif //CONCERTO_INPUT_HPP
