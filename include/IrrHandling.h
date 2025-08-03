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
#include "XEffects.h"

#include <queue>
#include <mutex>
#include <enet\enet.h>

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
	void setDriver(irr::video::E_DRIVER_TYPE type);
	void initScene();
	void capture();
	void makeNewDevice();
	void doFullscreen();
	void doWindowed();
	int getMemUsed();
	void end();
	void appLoop();
	void testLuaFunc(sol::object f);
	void doWriteTextureThreaded(irr::video::ITexture* texture, std::string name);
	void writeTextureToFile(irr::video::ITexture* texture, std::string name);
	void updateFPS();
	void AddCameraToQueue(irr::scene::ICameraSceneNode* cam, irr::scene::ISceneNode* forward, bool defaultRendering, bool renderGUI);
	void AddTransformToQueue(int type, irr::scene::ISceneNode* node, irr::core::vector3df transform);
	void HandleTransformQueue();
	void setCameraMatrix(irr::scene::ICameraSceneNode* c);
	void HandleCameraQueue();
	void displayMessage(std::string title, std::string message, int image);
	int m_frameLimit = 60;
	float dt;
	bool didEnd = false;
	bool defaultExclude = false;
	bool renderedGUI = false;
	int lights = 0;
	bool legacyDrawing = false;

	irr::video::E_DRIVER_TYPE driverType = irr::video::EDT_DIRECT3D9;
	int width = 640;
	int height = 480;
	int fullscreen = false;
	int vSync = false;
	int stencil = false;
	int posX = 0;
	int posY = 0;
	int fps = 0;

	std::string key = "";
	bool verbose = false;

	// Render queue
	std::queue<CameraToQueue> cameraQueue;

	// Transform queue
	std::queue<BatchedTransform> transformQueue;

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

	// XEffects
	E_FILTER_TYPE defaultShadowFiltering = E_FILTER_TYPE::EFT_8PCF;
	int defaultShadowResolution = 1024;

	irr::scene::ISceneNode* skydome = nullptr;
	irr::video::SColor backgroundColor = (255, 100, 101, 140);
};