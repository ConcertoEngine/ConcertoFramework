//
// Created by arthur on 18/08/2022.
//

#ifndef CONCERTO_EVENT_HPP
#define CONCERTO_EVENT_HPP

#include <variant>
#include <string>

#include "Concerto/Graphics/Core/Window/Key.hpp"

namespace cct
{
	enum class TriggerType
	{
		Released = 0,
		Pressed = 1,
	};

	struct KeyEvent
	{
		Key key;
		TriggerType triggerType;
	};

	struct MouseButton
	{
		enum class Button
		{
			Button1 = 0,
			Button2 = 1,
			Button3 = 2,
			Button4 = 3,
			Button5 = 4,
			Button6 = 5,
			Button7 = 6,
			Button8 = 7,
			Left = Button1,
			Right = Button2,
			Middle = Button3,
		};
		Button button;
		TriggerType triggerType;
	};

	struct MouseMove
	{
		double deltaX;
		double deltaY;
		double x;
		double y;
	};

	struct MouseWheel
	{
		float delta;
		int x;
		int y;
	};

	struct MouseEvent
	{
		enum class Type
		{
			Button,
			Moved,
			Wheel
		};
		Type type;
		MouseButton button;
		MouseMove mouseMove;
		MouseWheel mouseWheel;
	};

	struct Event
	{
		enum class Type
		{
			Key,
			Mouse,
			Custom
		};

		Type type;
		std::string name;
		std::variant<KeyEvent, MouseEvent, float, bool, int, std::string> data;
	};
}
#endif //CONCERTO_EVENT_HPP
