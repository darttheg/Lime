#pragma once
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>
#include "IrrHandling.h"
#include <sol/sol.hpp>

enum struct MESSAGE_TYPE : int {
	NORMAL = 0,
	WARNING = 1,
	LUA_WARNING = 2,
	NETWORK_VERBOSE = 3
};

class DebugConsole
{
private:
	const char* getTime();
	void abruptEnd();
public:
	void makeConsole();
	void sendMsg(const char* msg, MESSAGE_TYPE type);
	void writeOutput();

	bool enabled = false;
	bool doOutput = false;

	std::string out;
};

inline DebugConsole dConsole;

