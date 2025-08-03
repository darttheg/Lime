#pragma once
#include <filesystem>
#include <sol/sol.hpp>
#include "IrrHandling.h"

#define SOL_ALL_SAFETIES_ON 1
#define SOL_EXCEPTIONS_SAFE_PROPAGATION 1

class LuaLime
{
private:
	int ensureStructure();
public:
	int initLua(irr::scene::ISceneManager* smgr, irr::video::IVideoDriver* driver);
};

inline sol::state* lua;