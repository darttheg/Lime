#include "FunctionsImports.h"
#include "PerlinHeader.h"

namespace Bind {
	PerlinObject p;

	irr::video::E_DRIVER_TYPE getDriverType(int driverType) {
		switch (driverType) {
		case 0: return irr::video::EDT_NULL;
		case 1: return irr::video::EDT_SOFTWARE;
		case 2: return irr::video::EDT_BURNINGSVIDEO;
		case 3: return irr::video::EDT_DIRECT3D8;
		case 4: return irr::video::EDT_DIRECT3D9;
		case 5: return irr::video::EDT_OPENGL;
		default: return irr::video::EDT_NULL;
		}
	}

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

	void fullscreenWindow(bool var) {
		var ? irrHandler->doFullscreen() : irrHandler->doWindowed();
	}

	void setTitle(const std::string& title) {
		irrHandler->windowTitle = title;
		if (device) {
			device->setWindowCaption(irr::core::stringw(irrHandler->windowTitle.c_str()).c_str());
		}
	}

	std::string getTitle() {
		return irrHandler->windowTitle;
	}

	void setWindowPosition(const Vector2D& position) {
		if (irrHandler->glfwWindow) {
			glfwSetWindowPos(irrHandler->glfwWindow, static_cast<int>(position.x), static_cast<int>(position.y));
		}
	}

	Vector2D getWindowSize() {
		if (!irrHandler->glfwWindow)
			return Vector2D();

		int width, height;
		glfwGetWindowSize(irrHandler->glfwWindow, &width, &height);
		return Vector2D(static_cast<float>(width), static_cast<float>(height));
	}

	bool setWindowSize(const Vector2D& sizes) {
		if (sizes.x > 0 && sizes.y > 0 && irrHandler->glfwWindow) {
			int winX, winY;
			glfwGetWindowPos(irrHandler->glfwWindow, &winX, &winY);

			int oldW, oldH;
			glfwGetWindowSize(irrHandler->glfwWindow, &oldW, &oldH);
			glfwSetWindowSize(irrHandler->glfwWindow, static_cast<int>(sizes.x), static_cast<int>(sizes.y));

			int deltaW = static_cast<int>(sizes.x) - oldW;
			int deltaH = static_cast<int>(sizes.y) - oldH;
			glfwSetWindowPos(irrHandler->glfwWindow, winX - deltaW / 2, winY - deltaH / 2);

			irrHandler->width = sizes.x;
			irrHandler->height = sizes.y;

			irrHandler->updateIrrRenderRes();

			// Skip delta on ACTUAL resize
			glfwGetWindowSize(irrHandler->glfwWindow, &winX, &winY);
			if ((winX != oldW) && (winY != oldH))
				receiver->skipDeltaOnResize = true;

			return true;
		}
		else if (sizes.x > 0 && sizes.y > 0) {
			irrHandler->width = sizes.x;
			irrHandler->height = sizes.y;
		}
		return false;
	}

	Vector2D getMonitorSize() {
	#ifdef _WIN32
		return Vector2D(
			static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
			static_cast<float>(GetSystemMetrics(SM_CYSCREEN))
		);
	#endif
	}

	void endApplication() {
		if (device && irrHandler)
			irrHandler->end();
	}

	bool isFocused() {
		if (device)
			return device->isWindowFocused();
		return false;
	}

	void makeResizable(bool enable) {
		if (!irrHandler) return;

		irrHandler->isResizable = enable;
		if (irrHandler->glfwWindow)
			glfwSetWindowAttrib(irrHandler->glfwWindow, GLFW_RESIZABLE, irrHandler->isResizable ? GLFW_TRUE : GLFW_FALSE);
	}

	int getElapsedTime() {
		if (device)
			return device->getTimer()->getTime();
		return 0;
	}

	void logConsole(const std::string& title, int intensity = 0) {
		dConsole.sendMsg(title.c_str(), (MESSAGE_TYPE)intensity);
	}

	void writeConsoleOutput(bool var) {
		dConsole.doOutput = var;
	}

	void showConsole(bool var) {
		dConsole.enabled = var;
	}

	void recreateDevice() {
		// irrHandler->makeNewDevice();
	}

	void setVerticalSync(bool enable) {
		irrHandler->limiter.setVSync(enable);
		irrHandler->vSync = enable;
	}

	void displayMessage(std::string title, std::string message, int image) {
		irrHandler->displayMessage(title, message, image);
	}

	void maintainAspectRatio(bool enable) {
		if (!irrHandler) return;

		irrHandler->maintainAspectRatio = enable;

		if (!irrHandler->glfwWindow) return;

		if (irrHandler->maintainAspectRatio)
			glfwSetWindowAspectRatio(irrHandler->glfwWindow, irrHandler->width, irrHandler->height);
		else
			glfwSetWindowAspectRatio(irrHandler->glfwWindow, GLFW_DONT_CARE, GLFW_DONT_CARE);
	}

	int getFrameRate() {
		return irrHandler ? irrHandler->fps : 0;
	}

	void setFrameRate(int fps) {
		if (fps >= 0 && irrHandler)
			irrHandler->limiter.setFPS(fps);
	}

	int getMemoryUsage() {
		if (device && irrHandler)
			return irrHandler->getMemUsed();
		return 0;
	}

	std::string getVersion() {
		return irrHandler->version;
	}

	void setMatchResSize(bool enable) {
		irrHandler->matchResSize = enable;
	}

	sol::object getCommandLineValue(const std::string& key) {
		if (irrHandler && irrHandler->cmdline.find(key) != irrHandler->cmdline.end()) {
			return sol::make_object((*lua), irrHandler->cmdline[key]);
		}
		return sol::nil;
	}

	void setWindowIcon(const Texture& icon) {
		irrHandler->imgIconPath = icon.getPath();
		if (!driver) {
			return;
		}

		irrHandler->setTitleBarIcon(irrHandler->imgIconPath);
	}

	void setEndOnError(bool v) {
		dConsole.endOnError = v;
	}

	void setNoiseSeed(uint32_t seed) {
		p.setSeed(seed);
	}

	int getNoiseSeed() {
		return p.seed;
	}

	void setNoiseOctaves(uint32_t octaves) {
		p.setOctaves(octaves);
	}

	int getNoiseOctaves() {
		return p.octaves;
	}

	float getNoiseValue(uint32_t dimension, float x, float y, float z) {
		return p.getValueAt(dimension, x, y, z);
	}
}

void bindApplication() {
	sol::table application = lua->create_named_table("Lime");

	sol::table noise = lua->create_named_table("noise");
	(*lua)["math"]["noise"] = noise;

	(*lua)["math"]["noise"]["setSeed"] = &Bind::setNoiseSeed;
	(*lua)["math"]["noise"]["setOctaves"] = &Bind::setNoiseOctaves;
	(*lua)["math"]["noise"]["get"] = &Bind::getNoiseValue;

	application["SetDriverType"] = &Bind::setDriverType;
	application["SetFullscreen"] = &Bind::fullscreenWindow;
	application["SetWindowTitle"] = &Bind::setTitle;
	application["GetWindowTitle"] = &Bind::getTitle;
	application["SetWindowPosition"] = &Bind::setWindowPosition;
	application["GetWindowSize"] = &Bind::getWindowSize;
	application["SetWindowSize"] = &Bind::setWindowSize;
	application["GetMonitorSize"] = &Bind::getMonitorSize;
	application["End"] = &Bind::endApplication;
	application["IsWindowFocused"] = &Bind::isFocused;
	application["SetResizable"] = &Bind::makeResizable;
	application["GetElapsedTime"] = &Bind::getElapsedTime;
	application["Log"] = &Bind::logConsole;
	application["SetConsoleVisible"] = &Bind::showConsole;
	application["SetWriteConsole"] = &Bind::writeConsoleOutput;
	application["RecreateDevice"] = &Bind::recreateDevice;
	application["SetVSync"] = &Bind::setVerticalSync;
	application["DisplayMessage"] = &Bind::displayMessage;
	application["SetAutoAspectRatio"] = &Bind::maintainAspectRatio;
	application["GetFrameRate"] = &Bind::getFrameRate;
	application["SetFrameRate"] = &Bind::setFrameRate;
	application["GetMemoryUsage"] = &Bind::getMemoryUsage;
	application["GetVersion"] = &Bind::getVersion;
	application["SetMatchResolutionToSize"] = &Bind::setMatchResSize;
	application["GetCommandLine"] = &Bind::getCommandLineValue;
	application["SetWindowIcon"] = &Bind::setWindowIcon;
	application["SetEndOnError"] = &Bind::setEndOnError;
}