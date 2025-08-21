#pragma once

#include <vector>
#include "LuaLime.h"

class Event {
private:
	std::vector<int> funcs;
public:
	Event();

	void add(sol::function f); // Add a function
	void clear(); // Clear list
	void run(); // Run Event
	int getSize();
};

void bindEvent();