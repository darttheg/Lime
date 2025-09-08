#include "Event.h"

Event::Event() {
}

Hook Event::hook(sol::function f) {
	f.push();
	int ref = luaL_ref((*lua), LUA_REGISTRYINDEX);
	funcs.push_back(ref);
	// We can get the function because it's sitting at the top of the registry after just being called.

	return Hook(shared_from_this(), ref);
}

bool Event::removeRef(int ref) {
	auto it = std::find(funcs.begin(), funcs.end(), ref);
	if (it == funcs.end())
		return false;

	luaL_unref((*lua), LUA_REGISTRYINDEX, ref);
	funcs.erase(it);
	return true;
}

void Event::clear() {
	for (int ref : funcs) {
		luaL_unref((*lua), LUA_REGISTRYINDEX, ref);
	}

	funcs.clear();
}

bool Event::empty() {
	return getSize() == 0;
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

	if (passc > 0)
		lua_pop((*lua), passc);
}

void bindEvent() {
	sol::usertype<Event> bindType = lua->new_usertype<Event>("Event",
		sol::no_constructor
	);

	bindType["hook"] = &Event::hook;
	bindType["clear"] = &Event::clear;
	bindType["run"] = &Event::run;
	bindType["length"] = &Event::getSize;

	bindType.set_function("new",
		sol::factories([]() {
			return std::make_shared<Event>();
			})
	);

	bindType[sol::meta_function::call] = &Event::run;
}

//// Hook ////

Hook::Hook() {
}

void Hook::unhook() {
	if (!hooked)
		return;

	if (auto e = myEvent.lock())
		e->removeRef(ref);

	ref = LUA_NOREF;
	hooked = false;
}

void bindHook() {
	sol::usertype<Hook> bindType = lua->new_usertype<Hook>("Hook",
		sol::constructors<Hook()>()
	);
	// Only receive this object when hooking to an Event so no constructors

	bindType["unhook"] = &Hook::unhook;
	bindType["getHooked"] = &Hook::isHooked;
}