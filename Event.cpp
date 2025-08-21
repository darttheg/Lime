#include "Event.h"

Event::Event() {
}

void Event::add(sol::function f) {
	f.push();
	funcs.push_back(luaL_ref((*lua), LUA_REGISTRYINDEX));
	// We can get the function because it's sitting at the top of the registry after just being called.
}

void Event::clear() {
	for (int ref : funcs) {
		luaL_unref((*lua), LUA_REGISTRYINDEX, ref);
	}

	funcs.clear();
}

void Event::run() {
	int argc = lua_gettop((*lua));
	int passc = (argc >= 1) ? (argc - 1) : 0;

	for (int ref : funcs) {
		lua_rawgeti((*lua), LUA_REGISTRYINDEX, ref); // Push callback function from registry onto stack

		// Starts at index 2 to exclude self
		for (int i = 2; i <= argc; ++i)
			lua_pushvalue((*lua), i);

		// Call Lua function with pushed arguments
		if (lua_pcall((*lua), passc, 0, 0) != LUA_OK)
			lua_pop((*lua), 1);
	}

	if (passc > 0) lua_pop((*lua), passc);
}

int Event::getSize() {
	return funcs.size();
}

void bindEvent() {
	sol::usertype<Event> bindType = lua->new_usertype<Event>("Event",
		sol::constructors<Event()>()
	);

	bindType["add"] = &Event::add;
	bindType["clear"] = &Event::clear;
	bindType["run"] = &Event::run;
	bindType["length"] = &Event::getSize;
}