#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "LuaLime.h"
#include "Vector2D.h"
#include "Texture.h"
#include "Camera3D.h"
#include "DebugVisual.h"
#include "Sound.h"

typedef unsigned int u32;

using namespace irr;

namespace Warden {
	inline video::SColor fogColor = video::SColor(255, 138, 125, 81);
	inline video::E_FOG_TYPE fogType = video::EFT_FOG_LINEAR;
	inline float startDistance = 250.0f;
	inline float endDistance = 1000.0f;
	inline float fogDensity = 0.003f;
	inline bool pixelFog = true;
	inline bool rangeFog = false;
	inline std::string caption = "Lime Application";

	void logConsole(const std::string& title, int intensity) {
		dConsole.sendMsg(title.c_str(), (MESSAGE_TYPE)intensity);
	}

	// Get/set window caption
	std::string getTitle() {
		return caption;
	}

	void setTitle(const std::string& title) {
		caption = title;
		if (device) {
			device->setWindowCaption(irr::core::stringw(caption.c_str()).c_str());
		}
	}

	// Set window mode
	void fullscreenWindow(bool var) {
		var ? irrHandler->doFullscreen() : irrHandler->doWindowed();
	}

	irr::video::E_DRIVER_TYPE getDriverType(int driverType) {
		switch (driverType) {
		case 0: return irr::video::EDT_NULL;
		case 1: return irr::video::EDT_SOFTWARE;
		case 2: return irr::video::EDT_BURNINGSVIDEO;
		case 3: return irr::video::EDT_DIRECT3D8;
		case 4: return irr::video::EDT_DIRECT3D9;
		case 5: return irr::video::EDT_OPENGL;
		default: return irr::video::EDT_NULL; // Default to null driver if invalid
		}
	}

	// set driver type
	bool setDriverType(int driverType) {
		irr::video::E_DRIVER_TYPE type = getDriverType(driverType);
		if (device->isDriverSupported(type)) {
			irrHandler->setDriver(type);
			return true;
		}
		return false;

		/*
			if (device->isDriverSupported(type)) {
				if (device) {
					device->closeDevice();
					device->drop();
					device = nullptr;
				}

				device = irr::createDevice(type, irr::core::dimension2d<u32>(irrHandler->width, irrHandler->height), 16, false, irrHandler->stencil, irrHandler->vSync, receiver);
			}

			return device != nullptr;
		*/
	}

	void setVerticalSync(bool enable) {
		if (irrHandler)
			irrHandler->vSync = enable;
	}

	void setShadows(bool enable) {
		if (irrHandler)
			irrHandler->stencil = enable;
	}

	// set window size
	bool setWindowSize(Vector2D& sizes) {
		if (sizes.x > 0 && sizes.y > 0) {
			irrHandler->width = sizes.x;
			irrHandler->height = sizes.y;
			return true;
		}
		return false;
	}

	Vector2D getWindowSize() {
		return driver ? Vector2D(driver->getScreenSize().Width, driver->getScreenSize().Height) : Vector2D();
	}

	// set window position
	void setWindowPosition(Vector2D& position, irr::IrrlichtDevice* device) {
		if (device) {

#ifdef _WIN32
			HWND hwnd = reinterpret_cast<HWND>(device->getVideoDriver()->getExposedVideoData().OpenGLWin32.HWnd);
			if (hwnd) {
				SetWindowPos(hwnd, nullptr, static_cast<int>(position.x), static_cast<int>(position.y), 0, 0,
					SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			}
#endif
		}
	}

	void recreateDevice() {
		irrHandler->makeNewDevice();
	}

	Vector2D getMonitorSize() {
#ifdef _WIN32
		// set irrHandler pos perhaps
		return Vector2D(
			static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
			static_cast<float>(GetSystemMetrics(SM_CYSCREEN))
		);
#endif
	}

	// Get/set frame rate
	int getFrameRate() {
		if (device && irrHandler)
			return irrHandler->fps;
		return 0;
	}

	void setFrameRate(int fps) {
		if (device && fps >= 0 && irrHandler)
			irrHandler->m_frameLimit = fps;
	}

	// Get memory usage
	int getMemoryUsage() {
		if (device && irrHandler)
			return irrHandler->getMemUsed();
		return 0;
	}

	// Get elapsed time in miliseconds
	int getElapsedTime() {
		if (device)
			return device->getTimer()->getTime();
		return 0;
	}

	// Exit application
	void endApplication() {
		if (device && irrHandler)
			irrHandler->end();
	}

	// Is window focused?
	bool isFocused() {
		if (device)
			return device->isWindowFocused();
		return false;
	}

	// Allow window to be resizable?
	void makeResizable(bool enable) {
		if (device)
			device->setResizable(enable);
	}

	// Set background color
	void setBackgroundColor(Vector4D& color) {
		if (driver && irrHandler) {
			irrHandler->backgroundColor = irr::video::SColor(color.w, color.x, color.y, color.z);
			effects->setClearColour(irrHandler->backgroundColor);
		}
	}

	void setLegacyDrawing(bool enable) {
		if (irrHandler)
			irrHandler->legacyDrawing = enable;
	}

	// Set skydome
	void setSkydome(Texture& sky) {
		if (!irrHandler || !device)
			return;
		if (irrHandler->skydome) {
			irrHandler->skydome->remove();
		}

		irrHandler->skydome = smgr->addSkyDomeSceneNode(sky.texture, 16, 8, 0.9, 2.0, 100.0);
	}

	void setSkydomeParams(u32 resX, u32 resY, float texPercent, float spherePercent, float radius) {
		irr::video::ITexture* cur = nullptr;
		if (!irrHandler || !device)
			return;
		if (irrHandler->skydome) {
			if (irrHandler->skydome->getMaterialCount() > 0) {
				cur = irrHandler->skydome->getMaterial(0).getTexture(0);
			}
			irrHandler->skydome->remove();
		}
		if (cur)
			irrHandler->skydome = smgr->addSkyDomeSceneNode(cur, resX, resY, texPercent, spherePercent, radius);
	}

	// Get number of objects
	int getObjectCount() {
		if (!irrHandler || !device)
			return 0;

		return smgr->getRootSceneNode()->getChildren().getSize();
	}

	// Show cursor
	void showCursor(bool var) {
		if (device)
			device->getCursorControl()->setVisible(var);
	}

	// Move cursor
	void setCursorPosition(Vector2D pos) {
		if (device)
			device->getCursorControl()->setPosition(irr::core::vector2di(pos.x, pos.y));
	}

	bool addArchive(const std::string& path) {
		if (device) {
			device->getFileSystem()->addFileArchive(path.c_str());
			return true;
		}
		return false;
	}

	std::unordered_map<int, bool> tblToMap(sol::table luaTable) {
		std::unordered_map<int, bool> hashMap;

		for (auto& pair : luaTable) {
			sol::optional<int> key = pair.first.as<sol::optional<int>>();
			if (key)
				hashMap[key.value()] = true;
		}

		return hashMap;
	}

	sol::table fireRaypick(Vector3D start, Vector3D end, float debugLifetime, sol::table exclusion = nullptr) {
		scene::ISceneCollisionManager* collisionManager = smgr->getSceneCollisionManager();
		core::line3d<f32> ray(core::vector3df(start.x, start.y, start.z), core::vector3df(end.x, end.y, end.z));

		/*
		if (exclusion)
			collisionManager->setExcludeIDs(tblToMap(exclusion));
		*/

		core::vector3df hitPosition;
		core::triangle3df hitTriangle;
		scene::ISceneNode* pickedNode = collisionManager->getSceneNodeAndCollisionPointFromRay(
			ray, hitPosition, hitTriangle, false);

		sol::table result = lua->create_table();

		if (pickedNode) {
			Vector3D normal = Vector3D(hitTriangle.getNormal().X, hitTriangle.getNormal().Y, hitTriangle.getNormal().Z);
			video::SMaterial material = pickedNode->getMaterial(0);
			Vector3D hit = Vector3D(hitPosition.X, hitPosition.Y, hitPosition.Z);
			result["ID"] = pickedNode->getID();
			result["normal"] = normal;
			result["materialID"] = material.ID;
			result["hitPosition"] = hit;
		}
		else {
			result["ID"] = -1;
			result["normal"] = Vector3D(0,1,0);
			result["materialID"] = -1;
			result["hitPosition"] = end;
		}

		if (debugLifetime > 0) {
			DebugSceneNode* d = new DebugSceneNode(mainCamera, smgr, 0, DebugType::RAY_PICK);
			d->raypick_start = start;
			d->raypick_end = pickedNode ? Vector3D(hitPosition.X, hitPosition.Y, hitPosition.Z) : end;
			d->raypick_hit = pickedNode ? true : false;
			d->raypick_life = debugLifetime;
		}

		//collisionManager->clearExcludeIDs();

		return result;
	}

	sol::table fireRaypick2D(Vector2D screenCoord, Vector3D end) {
		scene::ISceneCollisionManager* collisionManager = smgr->getSceneCollisionManager();
		core::line3d<f32> ray = collisionManager->getRayFromScreenCoordinates(
			core::position2di(screenCoord.x, screenCoord.y), smgr->getActiveCamera());
		ray.end = core::vector3df(end.x, end.y, end.z);

		core::vector3df hitPosition;
		core::triangle3df hitTriangle;
		scene::ISceneNode* pickedNode = collisionManager->getSceneNodeAndCollisionPointFromRay(
			ray, hitPosition, hitTriangle);

		sol::table result = lua->create_table();

		if (pickedNode) {
			Vector3D normal = Vector3D(hitTriangle.getNormal().X, hitTriangle.getNormal().Y, hitTriangle.getNormal().Z);
			video::SMaterial material = pickedNode->getMaterial(0);
			Vector3D hit = Vector3D(hitPosition.X, hitPosition.Y, hitPosition.Z);
			result["ID"] = pickedNode->getID();
			result["normal"] = normal;
			result["materialID"] = material.ID;
			result["hitPosition"] = hit;
		}
		else {
			result["ID"] = -1;
			result["normal"] = Vector3D(0, 1, 0);
			result["materialID"] = -1;
			result["hitPosition"] = end;
		}

		return result;
	}

	void showConsole(bool var) {
		dConsole.enabled = var;
	}

	void writeConsoleOutput(bool var) {
		dConsole.doOutput = var;
	}

	// INPUT

	// Input binds
	sol::table getMouseState() {
		if (receiver) {
			return receiver->getMouseState();
		}
		return sol::table();
	}

	sol::table getControllerState() {
		if (receiver) {
			return receiver->getControllerState();
		}
		return sol::table();
	}

	bool isKeyDown(int c) {
		if (receiver && c >= 0 && c < irr::KEY_KEY_CODES_COUNT) {
			return receiver->isKeyDown(static_cast<irr::EKEY_CODE>(c));
		}
		return false;
	}

	void isControllerConnected() {
		core::array<SJoystickInfo> joystickInfo;
		device->activateJoysticks(joystickInfo);
	}

	// FOG

	// Apply fog settings
	void applyFogSettings() {
		driver->setFog(
			fogColor,
			fogType,
			startDistance,
			endDistance,
			fogDensity,
			pixelFog,
			rangeFog
		);
	}

	// Set fog distances (start and end)
	void setFogDistances(Vector2D dist) {
		startDistance = dist.x;
		endDistance = dist.y;
		applyFogSettings();
	}

	// Set fog color
	void setFogColor(Vector4D color) {
		fogColor = video::SColor(static_cast<u32>(color.w), static_cast<u32>(color.x), static_cast<u32>(color.y), static_cast<u32>(color.z));
		applyFogSettings();
	}

	// Set fog type (linear, exponential, or exponential)
	void setFogType(int i) {
		switch (i) {
		case 0: fogType = video::EFT_FOG_LINEAR; break;
		case 1: fogType = video::EFT_FOG_EXP; break;
		case 2: fogType = video::EFT_FOG_EXP2; break;
		default: fogType = video::EFT_FOG_LINEAR; break;
		}
		applyFogSettings();
	}

	// Set fog density (for exponential fog types)
	void setFogDensity(float density) {
		fogDensity = density;
		applyFogSettings();
	}

	// Enable or disable per-pixel fog
	void setFogPixel(bool var) {
		pixelFog = var;
		applyFogSettings();
	}

	// Use range-based fog for better depth calculations, more expensive however
	void useDistanceForFog(bool var) {
		rangeFog = var;
		applyFogSettings();
	}

	void setFogSettings(Vector2D dist, Vector4D color, int i, float density, bool var, bool var1) {
		startDistance = dist.x;
		endDistance = dist.y;
		fogColor = video::SColor(static_cast<u32>(color.w), static_cast<u32>(color.x), static_cast<u32>(color.y), static_cast<u32>(color.z));
		switch (i) {
		case 0: fogType = video::EFT_FOG_LINEAR; break;
		case 1: fogType = video::EFT_FOG_EXP; break;
		case 2: fogType = video::EFT_FOG_EXP2; break;
		default: fogType = video::EFT_FOG_LINEAR; break;
		}
		fogDensity = density;
		pixelFog = var;
		rangeFog = var1;
		applyFogSettings();
	}

	// Shadows
	void setAmbientColor(const Vector4D& color) {
		smgr->setAmbientLight(video::SColorf(static_cast<u32>(color.x) / 255.0f, static_cast<u32>(color.y) / 255.0f, static_cast<u32>(color.z) / 255.0f, static_cast<u32>(color.w) / 255.0f));
		effects->setAmbientColor(SColor(static_cast<u32>(color.w), static_cast<u32>(color.x), static_cast<u32>(color.y), static_cast<u32>(color.z)));
	}

	void setShadowColor(const Vector4D& color) {
		smgr->setShadowColor(video::SColor(static_cast<u32>(color.x), static_cast<u32>(color.y), static_cast<u32>(color.z), static_cast<u32>(color.w)));
	}

	void setShadowOpacity(int opacity) {
		SColor s = smgr->getShadowColor();
		s.setAlpha(opacity);
		smgr->setShadowColor(s);
	}

	void defaultExclude(bool enable) {
		if (device && effects)
			irrHandler->defaultExclude = enable;
	}

	// Sound
	int play2DSound(const std::string& filePath, bool loop = false) {
		return soundManager->play2DSound(filePath, loop);
	}

	int play3DSound(const std::string& filePath, Vector3D src, bool loop = false) {
		return soundManager->play3DSound(filePath, src, loop);
	}

	void play2DSoundOnChannel(int channel, const std::string& filePath, bool loop = false) {
		soundManager->play2DSoundOnChannel(channel, filePath, loop);
	}

	void play3DSoundOnChannel(int channel, const std::string& filePath, Vector3D src, bool loop = false) {
		soundManager->play3DSoundOnChannel(channel, filePath, src, loop);
	}

	void stopChannel(int channel) {
		soundManager->stopChannel(channel);
	}

	void pauseChannel(int channel, bool paused) {
		soundManager->pauseChannel(channel, paused);
	}

	void loopChannel(int channel, bool loop) {
		soundManager->loopChannel(channel, loop);
	}

	void stopAllSounds() {
		soundManager->stopAllSounds();
	}

	bool preloadSound(std::string path) {
		return soundManager->preloadSound(path);
	}

	void setListenerPosition(Vector3D pos, Vector3D forward) {
		soundManager->setListenerPosition(pos, forward);
	}

	bool isChannelFree(int channel) {
		return soundManager->isChannelFree(channel);
	}

	int getNextAvailableChannel() {
		return soundManager->getNextAvailableChannel();
	}

	void resetChannelFX(int channel) {
		soundManager->resetChannelFX(channel);
	}

	void setPitch(int channel, float pitch) {
		soundManager->setPitch(channel, pitch);
	}

	void setVolume(int channel, float volume) {
		soundManager->setVolume(channel, volume);
	}

	void setPan(int channel, float pan) {
		soundManager->setPan(channel, pan);
	}

	void setDistortionEffect(int channel, int effect, bool enable, sol::table params) {
		soundManager->setDistortionEffect(channel, effect, enable, params);
	}

	int validChannel(int i) {
		return soundManager->validChannel(i);
	}

	void setPlaybackSpeed(int channel, float spd) {
		soundManager->setPlaybackSpeed(channel, spd);
	}

	void setPlayPosition(int channel, int ms) {
		soundManager->setPlayPosition(channel, ms);
	}

	std::string printChannelList() {
		return soundManager->printChannelList();
	}

	void setDopplerParameters(float dop, float dist) {
		soundManager->setDopplerParameters(dop, dist);
	}

	void setChannelVelocity(int channel, float vel) {
		soundManager->SetChannelVelocity(channel, vel);
	}

	void setChannelPosition3D(int channel, const Vector3D& pos) {
		soundManager->setChannelPosition3D(channel, pos);
	}

	void setChannelDistance(int channel, float distance) {
		soundManager->setMinDistance(channel, distance);
	}

	// Fonts
	bool embedFont(const std::string& fontPath) {
		std::string fontName = std::filesystem::path(fontPath).stem().string();

		if (fontCache.find(fontName) != fontCache.end())
			return true;

		gui::IGUIFont* font = guienv->getFont(fontPath.c_str());
		if (!font)
			return false;

		fontCache[fontName] = font;
		return true;
	}

	bool setDefaultFont(const std::string& fontName) {
		if (device) {
			auto it = fontCache.find(fontName);
			if (it != fontCache.end()) {
				irr::gui::IGUISkin* skin = guienv->getSkin();
				skin->setFont(it->second);
				defaultFont = it->first;
				return true;
			}
		}
		return false;
	}

	std::string getFontList() {
		std::string result;
		for (const auto& pair : fontCache) {
			result += pair.first + ", ";
		}
		if (!result.empty()) {
			result.pop_back();
			result.pop_back();
		}
		return result;
	}

	// Other World Stuff
	Vector2D toScreenPosition(const Vector3D& pos) {
		irr::core::vector3df world = irr::core::vector3df(pos.x, pos.y, pos.z);
		irr::core::vector2di screen = smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(world, smgr->getActiveCamera());

		return Vector2D(screen.X, screen.Y);
	}

	Texture renderCameraOutput(const Camera3D& c, const Vector2D& size, bool renderGUI) {
		irr::video::ITexture* tx = 0;

		irr::scene::ICameraSceneNode* cur = c.camera;
		if (!cur)
			cur = mainCamera;

		if (device && cur && driver->queryFeature(video::EVDF_RENDER_TO_TARGET)) {
			tx = driver->addRenderTargetTexture(core::dimension2d<u32>(size.x, size.y), "RTT1");

			driver->beginScene(true, true, irrHandler->backgroundColor);

			irrHandler->setCameraMatrix(cur);

			smgr->setActiveCamera(cur);

			if (irrHandler->legacyDrawing) {
				driver->setRenderTarget(tx, true, true, irrHandler->backgroundColor);
				smgr->drawAll();
			} else
				effects->update();

			if (renderGUI)
				guienv->drawAll();
		}

		// Draw scene again, but does not account for queued cameras
		smgr->setActiveCamera(mainCamera);
		driver->setRenderTarget(0, true, true, irrHandler->backgroundColor);

		Texture tex = Texture();
		tex.texture = tx;
		tex.path = "Render Target Texture";

		return tex;
	}

	void clearScene(bool includeModels) {
		if (smgr && device) {
			smgr->clear();
			if (includeModels)
				smgr->getMeshCache()->clear();
		}
	}

	void clearGUI() {
		if (guienv && device)
			guienv->clear();
	}

	void addPPX(std::string path) {
		if (effects)
			effects->addPostProcessingEffectFromFile(path.c_str());
	}

	void setDefaultShadowFiltering(int i) {
		if (irrHandler)
			irrHandler->defaultShadowFiltering = (E_FILTER_TYPE)i;
	}

	void setDefaultShadowResolution(int i) {
		switch (i) {
		case 0:
			i = 256;
			break;
		case 1:
			i = 512;
			break;
		case 2:
			i = 1024;
			break;
		case 3:
			i = 2048;
			break;
		case 4:
			i = 4096;
			break;
		default:
			i = 512;
			break;
		}

		if (irrHandler)
			irrHandler->defaultShadowResolution = i;
	}

	// 2D
	void setBilinearFiltering(bool enable) {
		if (device) {
			driver->getMaterial2D().TextureLayer[0].BilinearFilter = enable;
		}
	}

	void setAnisotropicFiltering(bool enable) {
		if (device) {
			driver->getMaterial2D().TextureLayer[0].AnisotropicFilter = enable;
		}
	}

	void setTrilinearFiltering(bool enable) {
		if (device) {
			driver->getMaterial2D().TextureLayer[0].TrilinearFilter = enable;
		}
	}

	void setAntiAliasing(int i) {
		if (device)
			driver->getMaterial2D().AntiAliasing = (irr::video::E_ANTI_ALIASING_MODE)i;
	}

	void queueGUI() {
		irrHandler->AddCameraToQueue(nullptr, nullptr, false, true);
	}
	
	// Preload
	bool preloadMesh(std::string filePath) {
		IAnimatedMesh* mesh = smgr->getMesh(filePath.c_str());
		if (mesh)
			mesh->grab();
		return mesh != nullptr;
	}

	bool preloadTexture(std::string filePath) {
		ITexture* tex = driver->getTexture(filePath.c_str());
		if (tex)
			tex->grab();
		return tex != nullptr;
	}

	bool unloadMesh(std::string filePath) {
		irr::scene::IMesh* mesh = smgr->getMesh(filePath.c_str());
		if (mesh) {
			smgr->getMeshCache()->removeMesh(mesh);
			return true;
		}
		return false;
	}

	bool unloadTexture(std::string filePath) {
		irr::video::ITexture* texture = driver->getTexture(filePath.c_str());
		if (texture) {
			driver->removeTexture(texture);
			return true;
		}
		return false;
	}

	void setLightManagementMode(int i) {
		i = irr::core::clamp<int>(i, 0, 2);
		if (lightManager)
			lightManager->setMode(i);

		if (i > 0)
			smgr->setLightManager(lightManager);
		else
			smgr->setLightManager(0);
	}

	void displayMessage(std::string title, std::string message, int image) {
		irrHandler->displayMessage(title, message, image);
	}

	// Network (Client)

	// Network (Server)
	void setVerbose(bool v) {
		networkHandler->setVerbose(v);
	}

	bool initializeNetworking() {
		return networkHandler ? networkHandler->initialize() : false;
	}

	void hostServer(std::string ip, int port, int maxClients, int maxChannels) {
		if (!networkHandler || !networkHandler->initialized) {
			if ((networkHandler->verbose)) dConsole.sendMsg("Networking WARNING: Failed to host server: Networking is not initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
			return;
		}

		networkHandler->hostServer(ip, port, maxClients, maxChannels);
	}

	bool isHostingServer() {
		return networkHandler ? networkHandler->isHosting() : false;
	}

	int getIP() {
		return networkHandler ? networkHandler->getServerIP() : 0;
	}

	int getPort() {
		return networkHandler ? networkHandler->getPort() : 0;
	}

	void setBandwidthLimits(int incoming, int outgoing) {
		if (networkHandler) networkHandler->setBandwidthLimit(incoming, outgoing); else if (!(networkHandler->initialized)) dConsole.sendMsg("Networking WARNING: Failed to set bandwidth limits: Networking is not initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
	}

	bool shutdownNetworking() {
		return networkHandler ? networkHandler->shutdown() : false;
	}

	bool stopHosting() {
		return networkHandler ? networkHandler->stopHosting() : false;
	}

	bool createClient(int outgoing, int channels) {
		return networkHandler ? networkHandler->createClient(outgoing, channels) : false;
	}

	bool destroyClient() {
		return networkHandler ? networkHandler->destroyClient() : false;
	}

	void connectClient(std::string ad, int port, int channels) {
		if (networkHandler) networkHandler->connectClient(ad, port, channels);
	}

	void disconnectClient() {
		if (networkHandler) networkHandler->disconnectClient();
	}

	bool isClientConnected() {
		return networkHandler ? networkHandler->isClientConnected() : false;
	}

	int getPeerState(int peerID) {
		return networkHandler ? networkHandler->getPeerState(peerID) : -1;
	}

	int getPeerPing(int peerID) {
		return networkHandler ? networkHandler->getPeerState(peerID) : -1;
	}

	void forceDisconnectClient(int peerID, int reason) {
		if (networkHandler) networkHandler->forceDisconnectClient(peerID, reason);
	}

	void sendPacketToServer(int channel, const Packet& p, bool tcp) {
		if (networkHandler) networkHandler->sendPacketToServer(p, channel, tcp);
	}

	void sendPacketToPeer(int peerID, int channel, const Packet& p, bool tcp) {
		if (networkHandler) networkHandler->sendPacketToPeer(peerID, p, channel, tcp);
	}

	void sendPacketToAll(int channel, const Packet& p, bool tcp) {
		if (networkHandler) networkHandler->sendPacketToAll(p, channel, tcp);
	}
};

void bindWarden() {
	sol::table application = lua->create_named_table("Lime");
	sol::table world = lua->create_named_table("World");
	sol::table sound = lua->create_named_table("Sound");
	sol::table gui = lua->create_named_table("GUI");
	sol::table input = lua->create_named_table("Input");
	sol::table networkClient = lua->create_named_table("NetworkClient");
	sol::table networkServer = lua->create_named_table("NetworkServer");

	// application
	if (true) {
		application["SetDriverType"] = &Warden::setDriverType;
		application["SetFullscreen"] = &Warden::fullscreenWindow;
		application["SetCaption"] = &Warden::setTitle;
		application["GetCaption"] = &Warden::getTitle;
		application["SetWindowPosition"] = &Warden::setWindowPosition;
		application["GetWindowSize"] = &Warden::getWindowSize;
		application["SetWindowSize"] = &Warden::setWindowSize;
		application["GetMonitorSize"] = &Warden::getMonitorSize;
		application["EndApplication"] = &Warden::endApplication;
		application["IsWindowFocused"] = &Warden::isFocused;
		application["SetResizable"] = &Warden::makeResizable;
		application["GetElapsedTime"] = &Warden::getElapsedTime;
		application["Log"] = &Warden::logConsole;
		application["AddArchiveToMemory"] = &Warden::addArchive;
		application["SetShowConsole"] = &Warden::showConsole;
		application["SetWriteConsole"] = &Warden::writeConsoleOutput;
		application["RecreateDevice"] = &Warden::recreateDevice;
		application["SetVSync"] = &Warden::setVerticalSync;
		application["DisplayMessage"] = &Warden::displayMessage;
	}
	
	// world
	if (true) {
		world["GetFrameRate"] = &Warden::getFrameRate;
		world["SetFrameRate"] = &Warden::setFrameRate;
		world["GetMemoryUsage"] = &Warden::getMemoryUsage;
		world["SetSkydome"] = &Warden::setSkydome;
		world["SetSkydomeParameters"] = &Warden::setSkydomeParams;
		world["SetBackgroundColor"] = &Warden::setBackgroundColor;
		world["GetObjectCount"] = &Warden::getObjectCount;
		world["FireRaypick3D"] = &Warden::fireRaypick;
		world["FireRaypick2D"] = &Warden::fireRaypick2D;
		world["SetFogDistances"] = &Warden::setFogDistances;
		world["SetFogColor"] = &Warden::setFogColor;
		world["SetFogType"] = &Warden::setFogType;
		world["SetFogDensity"] = &Warden::setFogDensity;
		world["SetPixelFog"] = &Warden::setFogPixel;
		world["SetRangeFog"] = &Warden::useDistanceForFog;
		world["SetFogParameters"] = &Warden::setFogSettings;
		world["SetAmbientColor"] = &Warden::setAmbientColor;
		world["ConvertToScreenPosition"] = &Warden::toScreenPosition;
		world["SetShadows"] = &Warden::setShadows;
		world["GetRenderTexture"] = &Warden::renderCameraOutput;
		world["Clear"] = &Warden::clearScene;
		world["AddPostProcessingEffect"] = &Warden::addPPX;
		world["SetDefaultShadowFiltering"] = &Warden::setDefaultShadowFiltering;
		world["SetDefaultShadowResolution"] = &Warden::setDefaultShadowResolution;
		world["SetDefaultLightingExclusion"] = &Warden::defaultExclude;
		world["PreloadMesh"] = &Warden::preloadMesh;
		world["PreloadTexture"] = &Warden::preloadTexture;
		world["UnloadMesh"] = &Warden::unloadMesh;
		world["UnloadTexture"] = &Warden::unloadTexture;
		world["SetLegacyDrawing"] = &Warden::setLegacyDrawing;
		world["SetShadowColor"] = &Warden::setShadowColor;
		world["SetShadowOpacity"] = &Warden::setShadowOpacity;
		world["SetLightManagementMode"] = &Warden::setLightManagementMode;
	}

	// gui/2D images/text
	if (true) {
		gui["ImportFont"] = &Warden::embedFont;
		gui["SetDefaultFont"] = &Warden::setDefaultFont;
		gui["GetImportedFontsList"] = &Warden::getFontList;
		gui["SetBilinearFiltering"] = &Warden::setBilinearFiltering;
		gui["SetAnisotropicFiltering"] = &Warden::setAnisotropicFiltering;
		gui["SetTrilinearFiltering"] = &Warden::setTrilinearFiltering;
		gui["SetAntiAliasing"] = &Warden::setAntiAliasing;
		gui["Clear"] = &Warden::clearGUI;
		gui["Queue"] = &Warden::queueGUI;
	}

	// sound
	if (true) {
		sound["PlaySound2D"] = &Warden::play2DSound;
		sound["PlaySound3D"] = &Warden::play3DSound;
		sound["PlaySound2DOnChannel"] = &Warden::play2DSoundOnChannel;
		sound["PlaySound3DOnChannel"] = &Warden::play3DSoundOnChannel;
		sound["StopChannel"] = &Warden::stopChannel;
		sound["SetChannelPaused"] = &Warden::pauseChannel;
		sound["SetChannelLooped"] = &Warden::loopChannel;
		sound["StopAllChannels"] = &Warden::stopAllSounds;
		sound["PreloadSound"] = &Warden::preloadSound;
		sound["SetListenerPosition"] = &Warden::setListenerPosition;
		sound["ClearChannelEffects"] = &Warden::resetChannelFX;
		sound["SetChannelEffect"] = &Warden::setDistortionEffect;
		sound["SetChannelVolume"] = &Warden::setVolume;
		sound["SetChannelPitch"] = &Warden::setPitch;
		sound["SetChannelPan"] = &Warden::setPan;
		sound["GetChannelList"] = &Warden::printChannelList;
		sound["SetChannelPlaybackSpeed"] = &Warden::setPlaybackSpeed;
		sound["SetChannelPlaybackPosition"] = &Warden::setPlayPosition;
		sound["SetChannelVelocity"] = &Warden::setChannelVelocity;
		sound["SetDopplerEffectParameters"] = &Warden::setDopplerParameters;
		sound["SetChannelPosition3D"] = &Warden::setChannelPosition3D;
		sound["IsChannelEmpty"] = &Warden::isChannelFree;
		sound["SetChannelMinimumDistance"] = &Warden::setChannelDistance;
	}

	// input
	if (true) {
		input["IsKeyDown"] = &Warden::isKeyDown;
		input["GetMouseState"] = &Warden::getMouseState;
		input["GetControllerState"] = &Warden::getControllerState;
		input["CheckControllers"] = &Warden::isControllerConnected;
		input["SetMouseVisibility"] = &Warden::showCursor;
		input["SetMousePosition"] = &Warden::setCursorPosition;
	}

	// networkClient
	if (true) {
		networkClient["Initialize"] = &Warden::initializeNetworking;
		networkClient["SetVerbose"] = &Warden::setVerbose;
		networkClient["Create"] = &Warden::createClient;
		networkClient["Destroy"] = &Warden::destroyClient;
		networkClient["Connect"] = &Warden::connectClient;
		networkClient["Disconnect"] = &Warden::disconnectClient;
		networkClient["IsConnected"] = &Warden::isClientConnected;

		networkClient["SendPacketToServer"] = &Warden::sendPacketToServer;
	}

	// networkServer
	if (true) {
		networkServer["Initialize"] = &Warden::initializeNetworking;
		networkServer["SetVerbose"] = &Warden::setVerbose;
		networkServer["Host"] = &Warden::hostServer;
		networkServer["IsHosting"] = &Warden::isHostingServer;
		networkServer["StopHosting"] = &Warden::stopHosting;
		networkServer["GetIP"] = &Warden::getIP;
		networkServer["GetPort"] = &Warden::getPort;
		networkServer["SetBandwidthLimits"] = &Warden::setBandwidthLimits;
		networkServer["Shutdown"] = &Warden::shutdownNetworking;
		networkServer["GetPeerState"] = &Warden::getPeerState;
		networkServer["GetPeerPing"] = &Warden::getPeerPing;
		networkServer["DisconnectPeer"] = &Warden::forceDisconnectClient;

		networkServer["SendPacketToPeer"] = &Warden::sendPacketToPeer;
		networkServer["SendPacketToAll"] = &Warden::sendPacketToAll;
	}
}