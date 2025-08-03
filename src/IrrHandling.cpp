#include "IrrHandling.h"
#include "IrrManagers.h"
#include "LimeReceiver.h"
#include "Sound.h"

#include <filesystem>

namespace fs = std::filesystem;

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

struct windowState {
	int curWidth;
	int curHeight;
};

windowState window = { 640, 480 };

std::string getMainPath(const std::string& searchDirectory) {
	try {
		for (const auto& entry : fs::recursive_directory_iterator(searchDirectory)) {
			if (entry.is_regular_file() && entry.path().filename() == "main.lua") {
				return entry.path().string();
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error during directory traversal: " << e.what() << std::endl;
	}
	return "";
}

void IrrHandling::setDriver(irr::video::E_DRIVER_TYPE type) {
	driverType = type;
}

void IrrHandling::initScene()
{
	driverType = irr::video::EDT_DIRECT3D9;
	width = 640;
	height = 480;
	fullscreen = false;
	vSync = false;
	stencil = false;
	dConsole.enabled = false;
	dConsole.doOutput = false;
	posX = 0;
	posY = 0;
	fps = 0;

	window.curWidth = width;
	window.curHeight = height;

	LuaLime l;
	l.initLua(smgr, driver);

	// Is main.lua safe?
	std::string mainPath = getMainPath(".");
	if (mainPath == "") {
		dConsole.doOutput = true;
		std::string err = "main.lua could not be found!";
		dConsole.sendMsg(err.c_str(), MESSAGE_TYPE::WARNING);
		end();
		return;
	}

	try {
		sol::protected_function_result result = lua->safe_script_file(mainPath);
	}
	catch(const std::exception& e) {
		std::string err = e.what();
		
		dConsole.doOutput = true;
		dConsole.sendMsg(err.c_str(), MESSAGE_TYPE::WARNING);
		dConsole.writeOutput();

		err = "Lime encountered an error:\n" + err;

		std::wstring wStr = std::wstring(err.begin(), err.end());
		const wchar_t* wCharStr = wStr.c_str();

		MessageBox(nullptr, wStr.c_str(), TEXT("Lime Runtime Error"), MB_ICONEXCLAMATION);

		end();
		return;
	}

	if (dConsole.enabled)
		dConsole.makeConsole();

	dConsole.sendMsg("Lime started", MESSAGE_TYPE::NORMAL);

	receiver = new LimeReceiver();
	sound = irrklang::createIrrKlangDevice();
	soundManager = new SoundManager();

	device = irr::createDevice(driverType, dimension2d<u32>(width, height), 16, false, stencil, vSync, receiver);

	device->setWindowCaption(L"Lime Application");

	driver = device->getVideoDriver();
	effects = new EffectHandler(device, driver->getScreenSize(), false, true, false);
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();

	lightManager = new CLightManager(smgr);
	smgr->setLightManager(0);

	networkHandler = new NetworkHandler();

	appLoop();
}

void IrrHandling::capture() {
	switch (driverType) {
	case irr::video::E_DRIVER_TYPE::EDT_DIRECT3D8:
		SetCapture(reinterpret_cast<HWND>(device->getVideoDriver()->getExposedVideoData().D3D8.HWnd));
		break;
	case irr::video::E_DRIVER_TYPE::EDT_DIRECT3D9:
		SetCapture(reinterpret_cast<HWND>(device->getVideoDriver()->getExposedVideoData().D3D9.HWnd));
	case irr::video::E_DRIVER_TYPE::EDT_OPENGL:
		SetCapture(reinterpret_cast<HWND>(device->getVideoDriver()->getExposedVideoData().OpenGLWin32.HWnd));
	}
}

void IrrHandling::makeNewDevice() {
	if (device) {
		device->drop();
	}
	device = irr::createDevice(driverType, dimension2d<u32>(width, height), 16, false, stencil, vSync, receiver);
}

void IrrHandling::doFullscreen() {
	if (device) {
		device->maximizeWindow();
		HWND hwnd = reinterpret_cast<HWND>(device->getVideoDriver()->getExposedVideoData().OpenGLWin32.HWnd);
		LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
		style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		style |= WS_POPUP;
		SetWindowLongPtr(hwnd, GWL_STYLE, style);
		ShowWindow(hwnd, SW_MAXIMIZE);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	}
}

void IrrHandling::doWindowed() {
	if (device) {
		HWND hwnd = reinterpret_cast<HWND>(device->getVideoDriver()->getExposedVideoData().OpenGLWin32.HWnd);
		LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
		style |= (WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		style &= ~WS_POPUP;
		SetWindowLongPtr(hwnd, GWL_STYLE, style);
		SetWindowPos(hwnd, HWND_TOP, 100, 100, window.curWidth, window.curHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		ShowWindow(hwnd, SW_RESTORE);
		device->restoreWindow();
	}
}

int IrrHandling::getMemUsed() {
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
	int physMemUsedMB = physMemUsedByMe / (1024.0 * 1024.0);
	return physMemUsedMB;
}

void IrrHandling::end() {
	if (!didEnd) {
		dConsole.sendMsg("Ending application...", MESSAGE_TYPE::NORMAL);

		if (dConsole.doOutput) { // write console output to output.txt if enabled
			dConsole.writeOutput();
		}

		if (device) {
			device->closeDevice();
		}

		didEnd = true;
	}
}

void IrrHandling::appLoop() {

	// Events
	sol::protected_function luaOnStart = (*lua)["Lime"]["OnStart"];
	sol::protected_function luaOnUpdate = (*lua)["Lime"]["OnUpdate"];
	sol::protected_function luaOnEnd = (*lua)["Lime"]["OnEnd"];

	bool ranHandlers = false;

	lua->script("math.randomseed(os.time())");

	// Call start in main
	testLuaFunc((*lua)["Lime"]["OnStart"]);

	u32 then = device->getTimer()->getTime();

	f32 const frameDur = 1000.f / m_frameLimit;

	while (device->run()) {
		receiver->lastFocused = nullptr;
		const u32 now = device->getTimer()->getTime();
		dt = (now - then) / 16.667f;
		then = now;

		if (!ranHandlers) {
			ranHandlers = true;
			if (networkHandler)
				networkHandler->handle(irrHandler);
		}

		try {
			if ((*lua)["Lime"]["OnUpdate"].get_type() == sol::type::function) {
				sol::protected_function_result result = luaOnUpdate(dt);
				if (!result.valid())
				{
					sol::error err = result;
					dConsole.sendMsg(std::string(err.what()).c_str(), MESSAGE_TYPE::WARNING);
				}
			}
		}
		catch (const sol::error& e) {
			std::string err = e.what();

			dConsole.doOutput = true;
			dConsole.sendMsg(err.c_str(), MESSAGE_TYPE::WARNING);
			dConsole.writeOutput();

			err = "Lime encountered an error:\n" + err;

			std::wstring wStr = std::wstring(err.begin(), err.end());
			const wchar_t* wCharStr = wStr.c_str();

			MessageBox(nullptr, wStr.c_str(), TEXT("Lime Runtime Error"), MB_ICONEXCLAMATION);

			end();
			return;
		}

		if (mainCamera) {
			mainCamera->updateAbsolutePosition();
			mainCameraForward->updateAbsolutePosition();
			mainCamera->setTarget(mainCameraForward->getAbsolutePosition());
		}

		//HandleTransformQueue();

		HandleCameraQueue();

		if (!renderedGUI)
			guienv->drawAll();

		driver->endScene();

		updateFPS();

		// Set debug window caption
		if (dConsole.enabled) {
			stringw tmp(L"Lime Debugger [");
			tmp += driver->getName();
			tmp += L"] fps: ";
			tmp += fps;
			tmp += " | mem: ";
			tmp += getMemUsed();
			tmp += " MB";
			SetConsoleTitle(tmp.c_str());
		}

		renderedGUI = false;

		// Rounding issue with FPS
		f32 frameTime = device->getTimer()->getTime() - now;
		if (frameTime < frameDur)
			device->sleep((frameDur - frameTime) / 2.0);

		irrHandler->runEventTasks();
		irrHandler->runLuaTasks();
		irrHandler->runPacketToSend();
	}

	if (networkHandler)
		networkHandler->shutdown();

	testLuaFunc((*lua)["Lime"]["OnEnd"]);

	if (!didEnd)
		end();
}

void IrrHandling::testLuaFunc(sol::object f) {
	try {
		if (f.get_type() == sol::type::function) {
			sol::protected_function g = f.as<sol::protected_function>();
			sol::protected_function_result result = g(); // Doing Mesh:load(path) on its own will crash here.
			if (!result.valid())
			{
				sol::error err = result;
				dConsole.sendMsg(std::string(err.what()).c_str(), MESSAGE_TYPE::WARNING);
			}
		}
	}
	catch (const sol::error& e) {
		std::string err = e.what();

		dConsole.doOutput = true;
		dConsole.sendMsg(err.c_str(), MESSAGE_TYPE::WARNING);
		dConsole.writeOutput();

		err = "Lime encountered an error:\n" + err;

		std::wstring wStr = std::wstring(err.begin(), err.end());
		const wchar_t* wCharStr = wStr.c_str();

		MessageBox(nullptr, wStr.c_str(), TEXT("Lime Runtime Error"), MB_ICONEXCLAMATION);

		end();
		return;
	}
}

void IrrHandling::doWriteTextureThreaded(irr::video::ITexture* texture, std::string name) {
	std::thread(std::bind(&IrrHandling::writeTextureToFile, this, texture, name)).detach();
}

void IrrHandling::writeTextureToFile(irr::video::ITexture* texture, std::string name)
{
	if (!texture)
		return;

	void* data = texture->lock(irr::video::ETLM_READ_ONLY);
	if (!data)
		return;

	irr::video::IImage* image = driver->createImageFromData(texture->getColorFormat(), texture->getSize(), data, false);
	texture->unlock();

	if (!image)
		return;

	driver->writeImageToFile(image, name.c_str(), 99);

	image->drop();
}

void IrrHandling::updateFPS() {
	u32 currentTime = device->getTimer()->getTime();

	// Increment the frame counter
	++frameCount;

	// Calculate FPS once every second
	if (currentTime - lastTime >= 1000) {
		fps = frameCount / ((currentTime - lastTime) / 1000.0f);
		lastTime = currentTime;
		frameCount = 0;
	}
}

void IrrHandling::AddCameraToQueue(irr::scene::ICameraSceneNode* cam, irr::scene::ISceneNode* forward, bool defaultRendering, bool renderGUI)
{
	if (cam != mainCamera)
		cameraQueue.push(CameraToQueue(cam, forward, defaultRendering, renderGUI));
}

void IrrHandling::AddTransformToQueue(int type, irr::scene::ISceneNode* node, irr::core::vector3df transform) {
	transformQueue.push(BatchedTransform(type, node, transform));
}

void IrrHandling::HandleTransformQueue() {
	while (!transformQueue.empty()) {
		BatchedTransform b = transformQueue.front();
		irr::scene::ISceneNode* s = b.node;

		switch (b.type) {
		default:
			s->setPosition(b.transform);
			break;
		case 1: // rotation
			s->setRotation(b.transform);
			break;
		case 2: // scale
			s->setScale(b.transform);
			break;
		}

		transformQueue.pop();
	}
}

void IrrHandling::setCameraMatrix(irr::scene::ICameraSceneNode* c) {
	if (c->isTrulyOrthogonal) {
		irr::core::matrix4 orthoMat;
		float z = c->getFOV() * 180.0 / PI / 5.0;
		int width = device->getVideoDriver()->getScreenSize().Width;
		int height = device->getVideoDriver()->getScreenSize().Height;
		orthoMat.buildProjectionMatrixOrthoLH(width / z, height / z, c->getNearValue(), c->getFarValue());
		c->setProjectionMatrix(orthoMat, true);
	}
	else {
		irr::core::matrix4 perspectiveMat;
		float aspectRatio = (float)device->getVideoDriver()->getScreenSize().Width / (float)device->getVideoDriver()->getScreenSize().Height;
		perspectiveMat.buildProjectionMatrixPerspectiveFovLH(c->getFOV(), aspectRatio, c->getNearValue(), c->getFarValue());

		c->setProjectionMatrix(perspectiveMat, false);
	}
}

void IrrHandling::HandleCameraQueue() {
	driver->beginScene(true, true, backgroundColor);

	if (smgr->getActiveCamera()) {
		setCameraMatrix(smgr->getActiveCamera());

		if (legacyDrawing)
			smgr->drawAll();
		else {
			effects->update();
			effects->setClearColour(irr::video::SColor(0, 0, 0, 0));
		}
	}

	while (!cameraQueue.empty()) {
		CameraToQueue c = cameraQueue.front();

		setCameraMatrix(smgr->getActiveCamera());

		if (c.cam) {
			if (c.renderGUI && !renderedGUI) {
				guienv->drawAll();
				renderedGUI = true;
			}
			else {
				smgr->setActiveCamera(c.cam);
				c.cam->updateAbsolutePosition();
				c.forward->updateAbsolutePosition();
				c.cam->setTarget(c.forward->getAbsolutePosition());

				if (c.defaultRendering) {
					smgr->drawAll();
				}
				else {
					effects->update();
				}
			}
		}

		cameraQueue.pop();
	}

	smgr->setActiveCamera(mainCamera);
}

void IrrHandling::displayMessage(std::string title, std::string message, int image) {
	std::wstring nTitle = std::wstring(title.begin(), title.end());
	const wchar_t* nTitleC = nTitle.c_str();

	std::wstring nMessage = std::wstring(message.begin(), message.end());
	const wchar_t* nMessageC = nMessage.c_str();

	UINT icon = MB_OK;

	image = irr::core::clamp<int>(image, 0, 3);

	switch (image) {
	case 0:
		icon = MB_OK;
		break;
	case 1:
		icon = MB_ICONWARNING;
		break;
	case 2:
		icon = MB_ICONQUESTION;
		break;
	case 3:
		icon = MB_ICONINFORMATION;
		break;
	}

	MessageBox(nullptr, nMessageC, nTitleC, icon);
}

void IrrHandling::addPacketToSend(const PacketToSend& p) {
	if (p.p)
		packetOutQueue.push(p);
}

void IrrHandling::runPacketToSend() {
	bool doVerbose = verbose;
	tlqLock.lock();

	std::unordered_map<enet_uint16, ENetPeer*> peers = networkHandler->getPeers();

	while (!packetOutQueue.empty()) {
		PacketToSend task = packetOutQueue.front();

		if (task.p) {
			task.p->flags = task.tcp ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT;
			if (task.peerID == -1 && task.channel == -1) { // Server to all
				enet_host_broadcast(networkHandler->getHost(), task.channel, task.p);
				enet_host_flush(networkHandler->getHost());

				if (doVerbose) {
					std::string msg = "Packet of size ";
					msg += std::to_string(task.p->dataLength);
					msg += "B sent to all ";
					msg += " on channel ";
					msg += std::to_string(task.channel);
					msg += " via ";
					msg += task.tcp ? "TCP" : "UDP";
					dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
				}
			} else if (task.peerID != -1 && task.channel != -1) { // Server to peer
				ENetPeer* thisPeer = peers[task.peerID];
				if (!thisPeer) {
					if (doVerbose) {
						std::string msg = "Networking WARNING: Failed to send packet to peer with ID ";
						msg += std::to_string(task.peerID);
						msg += "; peer does not exist";
						dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
					}
					continue;
				}

				enet_peer_send(thisPeer, task.channel, task.p);
				enet_host_flush(networkHandler->getHost());

				if (doVerbose) {
					std::string msg = "Packet of size ";
					msg += std::to_string(task.p->dataLength);
					msg += "B sent to peer with ID ";
					msg += std::to_string(task.peerID);
					msg += " on channel ";
					msg += std::to_string(task.channel);
					msg += " via ";
					msg += task.tcp ? "TCP" : "UDP";
					dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
				}
			} else if (task.peerID == -1 && task.channel != -1) { // Peer to server
				if (!networkHandler->getPeer()) continue;

				enet_peer_send(networkHandler->getPeer(), task.channel, task.p);
				enet_host_flush(networkHandler->getClient());

				if (doVerbose) {
					std::string msg = "Packet of size ";
					msg += std::to_string(task.p->dataLength);
					msg += "B sent to server";
					msg += " on channel ";
					msg += std::to_string(task.channel);
					msg += " via ";
					msg += task.tcp ? "TCP" : "UDP";
					dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
				}
			}
		}

		packetOutQueue.pop();
	}

	tlqLock.unlock();
}

void IrrHandling::addLuaTask(sol::function f, sol::table args) {
	threadedLuaQueue.push({ f, args });
}

void IrrHandling::runLuaTasks() {
	//tlqLock.lock();

	while (!threadedLuaQueue.empty()) {
		std::pair<sol::function, sol::table> task = threadedLuaQueue.front();
		if (task.first.valid()) {
			std::vector<sol::object> args;
			if (task.second.valid()) {
				for (size_t i = 1; i <= task.second.size(); ++i) {
					args.push_back(task.second[i]);
				}
			}

			try {
				task.first(sol::as_args(args));
			}
			catch (const sol::error& e) {
				std::string err = e.what();

				dConsole.doOutput = true;
				dConsole.sendMsg(err.c_str(), MESSAGE_TYPE::WARNING);
				dConsole.writeOutput();

				err = "Lime encountered an error:\n" + err;

				std::wstring wStr = std::wstring(err.begin(), err.end());
				const wchar_t* wCharStr = wStr.c_str();

				MessageBox(nullptr, wStr.c_str(), TEXT("Lime Runtime Error"), MB_ICONEXCLAMATION);

				end();
			}
		}
		threadedLuaQueue.pop();
	}

	//tlqLock.unlock();
}

void IrrHandling::addEventTask(bool b, ENetEvent event) {
	eventOutQueue.push({ b, event });
}

void IrrHandling::runEventTasks() {
	bool doVerbose = verbose;
	tlqLock.lock();

	sol::protected_function SonPeerConnect = (*lua)["NetworkServer"]["OnClientConnect"];
	sol::protected_function SonPeerDisconnect = (*lua)["NetworkServer"]["OnClientDisconnect"];
	sol::protected_function SonPacketReceived = (*lua)["NetworkServer"]["OnPacketReceived"];
	sol::protected_function ConConnect = (*lua)["NetworkClient"]["OnConnect"];
	sol::protected_function ConDisconnect = (*lua)["NetworkClient"]["OnDisconnect"];
	sol::protected_function ConPacketReceived = (*lua)["NetworkClient"]["OnPacketReceived"];

	while (!eventOutQueue.empty()) {
		std::pair<bool, ENetEvent> task = eventOutQueue.front();
		
		ENetEvent event = task.second;
		if (task.first) { // Server
			switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				if (SonPeerConnect.valid()) {
					sol::table t = lua->create_table();
					t[1] = event.peer->incomingPeerID;
					t[2] = event.peer->address.host;

					addLuaTask(SonPeerConnect, t);
				}
				else {
					if (doVerbose) dConsole.sendMsg("Networking WARNING: A peer connected but NetworkServer.OnClientConnect is not declared", MESSAGE_TYPE::NETWORK_VERBOSE);
				}

				networkHandler->getPeerMap()[event.peer->incomingPeerID] = event.peer;

				if (doVerbose) {
					std::string msg = "Client joined presuming ID ";
					msg += std::to_string(event.peer->incomingPeerID);
					msg += " from IP ";
					msg += std::to_string(event.peer->address.host);
					dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
				}
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				if (SonPeerDisconnect.valid()) {
					sol::table t = lua->create_table();
					t[1] = event.peer->outgoingPeerID;
					t[2] = event.peer->address.host;

					addLuaTask(SonPeerDisconnect, t);
				}
				else {
					if (doVerbose) dConsole.sendMsg("Networking WARNING: A peer disconnected but NetworkServer.OnClientDisconnect is not declared", MESSAGE_TYPE::NETWORK_VERBOSE);
				}

				networkHandler->getPeerMap().erase(event.peer->outgoingPeerID);

				if (doVerbose) {
					std::string msg = "Client disconnected abandoning ID ";
					msg += std::to_string(event.peer->outgoingPeerID);
					msg += " from IP ";
					msg += std::to_string(event.peer->address.host);
					dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				if (SonPacketReceived.valid()) {
					sol::table t = lua->create_table();
					t[1] = event.channelID;
					t[2] = Packet(event.packet, event.peer->incomingSessionID);

					addLuaTask(SonPacketReceived, t);
				}
				else {
					if (doVerbose) dConsole.sendMsg("Networking WARNING: A packet was received but NetworkServer.OnPacketReceived is not declared", MESSAGE_TYPE::NETWORK_VERBOSE);
					enet_packet_destroy(event.packet);
				}
				break;
			}
		}
		else { // Client
			switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				if (ConConnect.valid())
					addLuaTask(ConConnect, sol::table());
				else {
					if (doVerbose) dConsole.sendMsg("Networking WARNING: Client connected but NetworkClient.OnConnect is not declared", MESSAGE_TYPE::NETWORK_VERBOSE);
				}

				//if (!n->getHost()) n->getPeerMap()[event.peer->incomingPeerID] = event.peer;

				if (doVerbose) {
					std::string msg = "Connected to server via client ";
					dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
				}
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				if (ConDisconnect.valid()) {
					sol::table t = lua->create_table();
					t[1] = event.data;
					addLuaTask(ConDisconnect, t);
				}
				else {
					if (doVerbose) dConsole.sendMsg("Networking WARNING: Client disconnected but NetworkClient.OnDisconnect is not declared", MESSAGE_TYPE::NETWORK_VERBOSE);
				}

				//if (!n->getHost()) n->getPeerMap().erase(event.peer->incomingPeerID);

				if (doVerbose) {
					std::string msg = "Disconnected from server as client, reason code ";
					msg += std::to_string(event.data);
					dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				if (ConPacketReceived.valid()) {
					sol::table t = lua->create_table();
					t[1] = event.channelID;
					t[2] = Packet(event.packet, event.peer->incomingPeerID);

					addLuaTask(ConPacketReceived, t);
				}
				else {
					if (doVerbose) dConsole.sendMsg("Networking WARNING: A packet was received but NetworkClient.OnPacketReceived is not declared", MESSAGE_TYPE::NETWORK_VERBOSE);
					enet_packet_destroy(event.packet);
				}
				break;
			}
		}

		eventOutQueue.pop();
	}

	tlqLock.unlock();
}
