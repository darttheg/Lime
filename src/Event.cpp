#include "Event.h"

Event::Event() {
}

void Event::add() {
	if (!lua_isfunction((*lua), -1)) {
		// Not a function
		return;
	}

	funcs.push_back(luaL_ref((*lua), LUA_REGISTRYINDEX));
	// We can get the function because it's sitting at the top of the registry after just being called.
}

void Event::clear() {
	for (int ref : funcs) {
		luaL_unref((*lua), LUA_REGISTRYINDEX, ref);
	}

	funcs.clear();
}

void Event::run(int argAmount = 0) {
	for (int ref : funcs) {
		lua_rawgeti((*lua), LUA_REGISTRYINDEX, ref);

		for (int i = 0; i < argAmount; i++)
			lua_pushvalue((*lua), -1 * (argAmount + 1)); // Bottom -> Up

		if (lua_pcall((*lua), argAmount, 0, 0) != LUA_OK)
			lua_pop((*lua), 1);
	}

	if (argAmount > 0)
		lua_pop((*lua), argAmount);
}

void bindEvent() {
	sol::usertype<Event> bindType = lua->new_usertype<Event>("Event",
		sol::constructors<Event()>()
	);

	bindType["add"] = &Event::add;
	bindType["clear"] = &Event::clear;
	bindType["run"] = &Event::run;
}