#include "LuaLime.h"
#include "IrrHandling.h"
#include "IrrManagers.h"
#include "DebugConsole.h"

#pragma comment(lib, "lua51.lib")

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace sol;
using namespace std;

#include "ParseCmd.h"

int main(int argc, char** argv) {
	IrrHandling i;
	irrHandler = &i;

	irrHandler->cmdline = parseArgs(argc, argv);

	i.initScene(); // app loop
}