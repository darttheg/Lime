#pragma once
#define _WINSOCKAPI_
#include <windows.h>
#include <winsock.h>
#include <iostream>
#include <sstream>
#include <direct.h>
#include <fstream>
#include <psapi.h>

#include <irrlicht.h>
#include <irrKlang.h>
#include <string>
#include "DebugConsole.h"
#include "LuaLime.h"

#include <queue>
#include <mutex>
#include <enet\enet.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "FrameLimiter.h"
#include "Preloader.h"

#include "DownloadGet.h"

struct PacketToSend {
public:
	PacketToSend(ENetPacket* pack, int chID, int pID, bool t) : p(pack), channel(chID), peerID(pID), tcp(t) {}
	ENetPacket* p;
	int channel;
	int peerID;
	bool tcp;
};

struct CameraToQueue {
public:
	CameraToQueue(irr::scene::ICameraSceneNode* c, irr::scene::ISceneNode* f, bool d, bool g) : cam(c), forward(f), defaultRendering(d), renderGUI(g), ortho(c->isTrulyOrthogonal) {};

	irr::scene::ICameraSceneNode* cam = nullptr;
	irr::scene::ISceneNode* forward = nullptr;
	bool defaultRendering = false;
	bool renderGUI = false;
	bool ortho = false;
};

struct BatchedTransform {
public:
	BatchedTransform(int t, irr::scene::ISceneNode* n, irr::core::vector3df v) : type(t), node(n), transform(v) {};

	int type = 0;
	irr::scene::ISceneNode* node = nullptr;
	irr::core::vector3df transform = irr::core::vector3df();
};

class IrrHandling
{
private:
	int lastTime = 0;
	int frameCount = 0;
public:
	// Version
	std::string version = "1.0.0-beta.3";
	//

	FrameLimiter limiter = FrameLimiter();

	void setDriver(irr::video::E_DRIVER_TYPE type);
	void initScene();
	void capture();
	void makeNewDevice();
	void doFullscreen();
	void doWindowed();
	int getMemUsed();
	void end();
	void appLoop();
	void doWriteTextureThreaded(irr::video::ITexture* texture, std::string name);
	void writeTextureToFile(irr::video::ITexture* texture, std::string name);
	void updateFPS();
	void AddCameraToQueue(irr::scene::ICameraSceneNode* cam, irr::scene::ISceneNode* forward, bool defaultRendering, bool renderGUI);
	void setCameraMatrix(irr::scene::ICameraSceneNode* c);
	void HandleCameraQueue();
	void displayMessage(std::string title, std::string message, int image);
	double dt = 0.0f;
	bool didEnd = false;
	bool renderedGUI = false;
	int lights = 0;

	irr::video::E_DRIVER_TYPE driverType = irr::video::EDT_DIRECT3D9;
	int width = 640;
	int height = 480;
	int fullscreen = false;
	int vSync = false;
	int stencil = false;
	int posX = 0;
	int posY = 0;
	int fps = 0;

	bool isResizable = false;
	bool maintainAspectRatio = false;

	std::string key = "";
	bool verbose = false;

	// Commandline
	std::unordered_map<std::string, std::string> cmdline;

	// Compatible3D map
	std::unordered_map<irr::scene::ISceneNode*, sol::table> comp3dmap;

	// Render queue
	std::queue<CameraToQueue> cameraQueue;

	// Lua function call queue
	std::queue<std::pair<sol::function, sol::table>> threadedLuaQueue;
	std::queue<std::pair<bool, ENetEvent>> eventOutQueue;
	std::queue<PacketToSend> packetOutQueue;
	std::mutex tlqLock;

	void addPacketToSend(const PacketToSend& p);
	void runPacketToSend();

	void addLuaTask(sol::function f, sol::table args);
	void runLuaTasks();

	void addEventTask(bool, ENetEvent);
	void runEventTasks();

	void onMaximizeWindow();
	void onRestoreWindow();

	void updateIrrRenderRes();
	void setTitleBarIcon(std::string path);
	bool matchResSize = true;

	// Window
	GLFWwindow* glfwWindow;
	std::string imgIconPath = "";
	std::string windowTitle = "Lime Application";

	irr::scene::ISceneNode* skydome = nullptr;
	irr::video::SColor backgroundColor = (255, 100, 101, 140);

	bool useHighLevelShaders = false;
	bool useCGShaders = false;

	// Preloader
	Preloader preload;

	// HTTP
	DownloadGet httpGetDownload;
	void drainHTTPEvents();
};