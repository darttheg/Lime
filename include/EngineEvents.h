#pragma once

#include "LuaLime.h"

class Event;

struct GeneralEvents {
	std::shared_ptr<Event> OnStart;
	std::shared_ptr<Event> OnUpdate;
	std::shared_ptr<Event> OnEnd;
};