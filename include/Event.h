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
	bool empty();

	template <class... Args>
	void engineRun(Args&&... args) {
		for (int ref : funcs) {
			lua_rawgeti((*lua), LUA_REGISTRYINDEX, ref);
			(sol::stack::push((*lua), std::forward<Args>(args)), ...);
			if (lua_pcall((*lua), sizeof...(Args), 0, 0) != LUA_OK) {
				size_t n = 0;
				const char* s = luaL_tolstring((*lua), -1, &n);
				std::string msg(s, n);
				lua_pop((*lua), 1);
				dConsole.postError(msg); 
			}
		}
	}
};

class Hook {
private:
	std::weak_ptr<Event> myEvent; // Weak pointer so if the Event dies, it won't crash
	int ref = LUA_NOREF;
	bool hooked = false; // True until :unhook()
public:
	Hook();
	Hook(std::shared_ptr<Event> e, int r) : myEvent(e), ref(r), hooked(true) {}

	sol::object unhook();
	bool isHooked() const { return hooked; }
};

void bindEvent();
void bindHook();