#include "IrrHandling.h"
#include "IrrManagers.h"
#include "LimeReceiver.h"

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

	lua->script("math.randomseed(os.time())");

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
		dConsole.endOnError = true;
		dConsole.postError(e.what());
		return;
	}

	if (dConsole.enabled)
		dConsole.makeConsole();

	dConsole.sendMsg("Lime started", MESSAGE_TYPE::NORMAL);

	receiver = new LimeReceiver();
	soundManager = new SoundManager();
	preload.soundEngine = soundManager->getEngine();

	///////////////////////
	// Create NEW device //
	///////////////////////
	
	//device = irr::createDevice(driverType, dimension2d<u32>(width, height), 16, false, stencil, vSync, receiver);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, isResizable ? GLFW_TRUE : GLFW_FALSE);

	glfwWindow = glfwCreateWindow(width, height, "Lime Application", nullptr, nullptr);

	if (maintainAspectRatio)
		glfwSetWindowAspectRatio(glfwWindow, width, height);
	else
		glfwSetWindowAspectRatio(glfwWindow, GLFW_DONT_CARE, GLFW_DONT_CARE);

	// Center
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int screenWidth = mode->width;
	int screenHeight = mode->height;

	int windowX = (screenWidth - width) / 2;
	int windowY = (screenHeight - height) / 2;

	glfwSetWindowPos(glfwWindow, windowX, windowY);

	// Set raw input mode to OFF
	glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Set automatically detect maximizing
	glfwSetWindowMaximizeCallback(glfwWindow, [](GLFWwindow* win, int maximized) {
		if (maximized) {
			irrHandler->onMaximizeWindow();
		}
		else {
			irrHandler->onRestoreWindow();
		}
	});

	glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height) {
		irrHandler->width = width;
		irrHandler->height = height;

		irrHandler->updateIrrRenderRes();
	});

	HWND hwnd = glfwGetWin32Window(glfwWindow);

	///////////////////////

	SIrrlichtCreationParameters params;
	params.DriverType = driverType;
	params.WindowSize = dimension2d<u32>(width, height);
	params.Bits = 16;
	params.Fullscreen = false;
	params.Stencilbuffer = stencil;
	params.Vsync = vSync;
	params.EventReceiver = receiver;
	//params.WindowId = hwnd;

	device = createDeviceEx(params);
	HWND hwndIrr = (HWND)device->getVideoDriver()->getExposedVideoData().D3D9.HWnd;
	ShowWindow(hwndIrr, SW_HIDE);

	SetParent(hwndIrr, hwnd);
	SetWindowLongPtr(hwndIrr, GWL_STYLE, WS_CHILD | WS_VISIBLE);
	SetWindowPos(hwndIrr, 0, 0, 0, width, height, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

	limiter.setVSync(vSync);

	setTitleBarIcon(irrHandler->imgIconPath); // Run in case title bar icon was changed before window creation
	device->setWindowCaption(irr::core::stringw(irrHandler->windowTitle.c_str()).c_str());

	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();
	gpu = driver->getGPUProgrammingServices();

	glfwSetWindowFocusCallback(glfwWindow, [](GLFWwindow* w, int focused) {
		HWND hwndIrr = (HWND)driver->getExposedVideoData().D3D9.HWnd;

		if (focused)
		{
			SendMessage(hwndIrr, WM_ACTIVATE, WA_ACTIVE, 0);
			SendMessage(hwndIrr, WM_SETFOCUS, 0, 0);
		}
		else
		{
			SendMessage(hwndIrr, WM_ACTIVATE, WA_INACTIVE, 0);
			SendMessage(hwndIrr, WM_KILLFOCUS, 0, 0);
		}
	});

	driver->beginScene(true, true, SColor(255, 0, 0, 0));
	driver->endScene();

	if (useCGShaders && !driver->queryFeature(video::EVDF_CG))
		useCGShaders = false;

	lightManager = new CLightManager(smgr);
	smgr->setLightManager(0);

	networkHandler = new NetworkHandler();
	physicsHandler = new PhysicsHandler();

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
	int physMemUsedMB = physMemUsedByMe / (1024.0 * 1024.0) - 10;

	if (physMemUsedMB < 0) physMemUsedMB = 0;
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

#include "LimeEvents.h"

void IrrHandling::appLoop() {
	// Call start in main
	Events::Lime::OnStart.get()->engineRun();

	bool tempMatch = matchResSize;
	matchResSize = true;
	updateIrrRenderRes();
	matchResSize = tempMatch;

	bool ranHandlers = false;

	preload.openSoundPreloading();

	while (device->run()) {
		dt = limiter.beginFrame();

		receiver->updateDeltaMouse(glfwWindow);

		if (!ranHandlers) {
			ranHandlers = true;
			if (networkHandler)
				networkHandler->handle(irrHandler);
		}

		try {
			Events::Lime::OnUpdate.get()->engineRun(dt * 60.0);
			drainHTTPEvents();
		}
		catch (const sol::error& e) {
			dConsole.postError(e.what());
		}

		// Update lastMouse
		receiver->updateLastMouse();

		if (mainCamera) {
			mainCamera->updateAbsolutePosition();
			mainCameraForward->updateAbsolutePosition();
			mainCamera->setTarget(mainCameraForward->getAbsolutePosition());
		}

		// Sound (listener position)
		if (soundManager)
			soundManager->update();

		// Physics
		if (physicsHandler)
			physicsHandler->update();

		HandleCameraQueue();

		if (!renderedGUI)
			guienv->drawAll();

		physicsHandler->world->debugDrawProperties(physicsHandler->drawProperties);

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

		irrHandler->runEventTasks();
		irrHandler->runLuaTasks();
		irrHandler->runPacketToSend();

		preload.pump(driver, smgr, device->getFileSystem());

		if (glfwWindowShouldClose(glfwWindow))
			device->closeDevice();
		else
			limiter.endFrame();
		
	}

	if (networkHandler)
		networkHandler->shutdown();

	Events::Lime::OnEnd.get()->engineRun();

	httpGetDownload.stop();

	preload.endSoundPreloading();

	if (!didEnd)
		end();
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
		// float aspectRatio = (float)device->getVideoDriver()->getScreenSize().Width / (float)device->getVideoDriver()->getScreenSize().Height;
		float aspectRatio = c->getAspectRatio();
		perspectiveMat.buildProjectionMatrixPerspectiveFovLH(c->getFOV(), aspectRatio, c->getNearValue(), c->getFarValue());

		c->setProjectionMatrix(perspectiveMat, false);
	}
}

void IrrHandling::HandleCameraQueue() {
	driver->beginScene(true, true, backgroundColor);

	if (smgr->getActiveCamera()) {
		setCameraMatrix(smgr->getActiveCamera());

		smgr->drawAll();

		physicsHandler->onRender();
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

				smgr->drawAll();
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
				dConsole.postError(e.what());
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

	while (!eventOutQueue.empty()) {
		std::pair<bool, ENetEvent> task = eventOutQueue.front();
		
		ENetEvent event = task.second;
		if (task.first) { // Server
			switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				if (!Events::Networking::SonPeerConnect.get()->empty()) { // Replace lua tasks with events
					Events::Networking::SonPeerConnect.get()->engineRun(event.peer->incomingPeerID, event.peer->address.host);
				}
				else {
					if (doVerbose) dConsole.sendMsg("WARNING: A peer connected but Event Network.Server.OnClientConnect is empty", MESSAGE_TYPE::NETWORK_VERBOSE);
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
				if (!Events::Networking::SonPeerDisconnect.get()->empty()) {
					Events::Networking::SonPeerDisconnect.get()->engineRun(event.peer->outgoingPeerID, event.peer->address.host);
				}
				else {
					if (doVerbose) dConsole.sendMsg("WARNING: A peer disconnected but Event Network.Server.OnClientDisconnect is empty", MESSAGE_TYPE::NETWORK_VERBOSE);
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
				if (!Events::Networking::SonPacketReceived.get()->empty()) {
					Events::Networking::SonPacketReceived.get()->engineRun(event.channelID, Packet(event.packet, event.peer->incomingSessionID));
				}
				else {
					if (doVerbose) dConsole.sendMsg("WARNING: A packet was received but Event Network.Server.OnPacketReceived is empty", MESSAGE_TYPE::NETWORK_VERBOSE);
					enet_packet_destroy(event.packet);
				}
				break;
			}
		}
		else { // Client
			switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				networkHandler->clientTrulyConnected = true;
				if (!Events::Networking::ConConnect.get()->empty())
					Events::Networking::ConConnect.get()->engineRun(this);
				else {
					if (doVerbose) dConsole.sendMsg("WARNING: Client connected but Event Network.Client.OnConnect is empty", MESSAGE_TYPE::NETWORK_VERBOSE);
				}

				//if (!n->getHost()) n->getPeerMap()[event.peer->incomingPeerID] = event.peer;

				if (doVerbose) {
					std::string msg = "Connected to server via client ";
					dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
				}
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				networkHandler->clientTrulyConnected = false;
				if (!Events::Networking::ConDisconnect.get()->empty()) {
					Events::Networking::ConDisconnect.get()->engineRun(event.data);
				}
				else {
					if (doVerbose) dConsole.sendMsg("WARNING: Client disconnected but Event Network.Client.OnDisconnect is empty", MESSAGE_TYPE::NETWORK_VERBOSE);
				}

				//if (!n->getHost()) n->getPeerMap().erase(event.peer->incomingPeerID);

				if (doVerbose) {
					std::string msg = "Disconnected from server as client, reason code ";
					msg += std::to_string(event.data);
					dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				if (!Events::Networking::ConPacketReceived.get()->empty()) {
					Events::Networking::ConPacketReceived.get()->engineRun(event.channelID, Packet(event.packet, event.peer->incomingPeerID));
				}
				else {
					if (doVerbose) dConsole.sendMsg("WARNING: A packet was received but Event Network.Client.OnPacketReceived is empty", MESSAGE_TYPE::NETWORK_VERBOSE);
					enet_packet_destroy(event.packet);
				}
				break;
			}
		}

		eventOutQueue.pop();
	}

	tlqLock.unlock();
}

void IrrHandling::onMaximizeWindow() {
	HWND hwnd = glfwGetWin32Window(glfwWindow);
	if (!hwnd) return;

	LONG style = GetWindowLongPtr(hwnd, GWL_STYLE);
	style &= ~(WS_CAPTION | WS_THICKFRAME);
	SetWindowLongPtr(hwnd, GWL_STYLE, style);

	LONG exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	exStyle &= ~(WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
	SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

	HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	if (GetMonitorInfo(hMonitor, &mi)) {
		int x = mi.rcMonitor.left;
		int y = mi.rcMonitor.top;
		int w = mi.rcMonitor.right - mi.rcMonitor.left;
		int h = mi.rcMonitor.bottom - mi.rcMonitor.top;

		SetWindowPos(hwnd, nullptr, x, y, w, h,
			SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		irrHandler->width = w;
		irrHandler->height = h;
		updateIrrRenderRes();
	}
}

void IrrHandling::onRestoreWindow() {
	HWND hwnd = glfwGetWin32Window(glfwWindow);
	if (!hwnd) return;

	LONG style = GetWindowLongPtr(hwnd, GWL_STYLE);
	style |= (WS_CAPTION | WS_THICKFRAME);
	SetWindowLongPtr(hwnd, GWL_STYLE, style);

	SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	updateIrrRenderRes();
}

void IrrHandling::updateIrrRenderRes() {
	if (matchResSize) {
		dimension2d<u32> newSize(static_cast<u32>(irrHandler->width), static_cast<u32>(irrHandler->height));
		driver->OnResize(newSize);
	}

	if (guienv) {
		auto* root = guienv->getRootGUIElement();
		root->setRelativePosition(core::rect<s32>(0, 0, (s32)irrHandler->width, (s32)irrHandler->height));
	}

	if (smgr->getActiveCamera())
		smgr->getActiveCamera()->setAspectRatio((f32)irrHandler->width / (f32)irrHandler->height);

	Events::Lime::OnResize.get()->engineRun();
}

// For glfw set window icon
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void IrrHandling::setTitleBarIcon(std::string path) {
	// Add invalid path check?
	GLFWimage images[1];
	images[0].pixels = stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4);
	if (images[0].pixels) {
		glfwSetWindowIcon(irrHandler->glfwWindow, 1, images);
		stbi_image_free(images[0].pixels);
	}
}

void IrrHandling::drainHTTPEvents() {
	std::vector<DownloadGet::Item> evs;
	httpGetDownload.drain(evs);

	for (auto& e : evs) {
		if (e.isDownload)
			Events::Networking::OnHTTPDownloadComplete.get()->engineRun(e.code, e.str);
		else
			Events::Networking::OnHTTPGet.get()->engineRun(e.code, e.str);
	}
}
