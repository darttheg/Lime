#pragma once

#include <vector>
#include "LuaLime.h"

class Event : public std::enable_shared_from_this<Event> {
private:
	std::vector<int> funcs;

	friend class Hook;
	bool removeRef(int ref);
public:
	Event();

	Hook hook(sol::function f); // Add a function
	void clear(); // Clear list
	void run(); // Run Event
	int getSize() const { return funcs.size(); }
};

class Hook {
private:
	std::weak_ptr<Event> myEvent; // Weak pointer so if the Event dies, it won't crash
	int ref = LUA_NOREF;
	bool hooked = false; // True until :unhook()
public:
	Hook();
	Hook(std::shared_ptr<Event> e, int r) : myEvent(e), ref(r), hooked(true) {}

	void unhook();
	bool isHooked() const { return hooked; }
};

void bindEvent();
void bindHook();