#pragma once

#include "Event.h"

// Struct for IGUIButton callbacks with events

struct ButtonEvents {
	std::shared_ptr<Event> hoverEvent;
	std::shared_ptr<Event> pressedEvent;

	ButtonEvents(std::shared_ptr<Event> a, std::shared_ptr<Event> b) 
		: hoverEvent(a), pressedEvent(b) {}
	ButtonEvents() = default;
};