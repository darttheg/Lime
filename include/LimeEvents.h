#pragma once

#include "Event.h"

namespace Events {
	namespace Lime {
		inline std::shared_ptr<Event> OnStart = std::make_shared<Event>();
		inline std::shared_ptr<Event> OnUpdate = std::make_shared<Event>();
		inline std::shared_ptr<Event> OnEnd = std::make_shared<Event>();
	}

	namespace Input {

	}
}

inline void initEvents() {
	(*lua)["Lime"]["OnStart"] = Events::Lime::OnStart;
	(*lua)["Lime"]["OnUpdate"] = Events::Lime::OnUpdate;
	(*lua)["Lime"]["OnEnd"] = Events::Lime::OnEnd;
}