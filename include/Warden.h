#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "LuaLime.h"
#include "Vector2D.h"
#include "Texture.h"
#include "Camera3D.h"
#include "DebugVisual.h"

#include "FunctionsApplication.h"
#include "FunctionsGUI.h"
#include "FunctionsInput.h"
#include "FunctionsNetworking.h"
#include "FunctionsSound.h"
#include "FunctionsWorld.h"
#include "FunctionsPhysics.h"

typedef unsigned int u32;
using namespace irr;

void bindWarden() {
	bindApplication();
	bindGUI();
	bindInput();
	bindNetworking();
	bindAudio();
	bindWorld();
	bindPhysics();
}