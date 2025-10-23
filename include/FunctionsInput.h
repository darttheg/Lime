#include "FunctionsImports.h"

#include "LimeEvents.h"

namespace Bind {
	sol::table getMouseState() {
		if (receiver) {
			return receiver->getMouseState();
		}
		return sol::table();
	}

	bool isKeyPressed(int c) {
		if (receiver && c >= 0 && c < irr::KEY_KEY_CODES_COUNT) {
			return receiver->isKeyPressed(static_cast<irr::EKEY_CODE>(c));
		}
		return false;
	}

	bool getMouseDown(int c = 0) {
		switch (c) {
		case 1:
			return receiver->MouseState.MiddleButtonDown;
			break;
		case 2:
			return receiver->MouseState.RightButtonDown;
			break;
		default:
			return receiver->MouseState.LeftButtonDown;
			break;
		}
	}

	Vector2D getCursorPosition() {
		double currentMouseX, currentMouseY;
		glfwGetCursorPos(irrHandler->glfwWindow, &currentMouseX, &currentMouseY);
		if (receiver)
			return Vector2D(currentMouseX, currentMouseY);
		return Vector2D();
	}

	void setRawInputMode(bool enable) {
		if (enable) {
			glfwSetInputMode(irrHandler->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(irrHandler->glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
		else {
			glfwSetInputMode(irrHandler->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	Vector2D getMouseDelta() {
		if (receiver->firstMouse) {
			receiver->firstMouse = false;
			return Vector2D(); //Vector2D(currentMouseX, currentMouseY);
		}
		return Vector2D(receiver->deltaX, receiver->deltaY);
	}

	// Show cursor
	void showCursor(bool var) {
		if (device)
			device->getCursorControl()->setVisible(var);
	}

	// Move cursor
	void setCursorPosition(Vector2D pos) {
		if (device) {
			device->getCursorControl()->setPosition(irr::core::vector2di(pos.getX(), pos.getY()));
			receiver->mouseX = pos.getX() - 1;
			receiver->mouseY = pos.getY() - 1;
		}
	}

	float getMouseWheelDelta() {
		return receiver ? receiver->MouseState.WheelDelta : 0.0;
	}

	// Joystick functions
	sol::table getJoystickState(int id = 0) {
		return lua->create_table();
	}

	bool isJoystickConnected(int id) {
		auto it = receiver->lastJoystickState.find(id);
		return it != receiver->lastJoystickState.end();
	}

	bool isJoystickButtonPressed(int id = 0, int buttonID = 0) {
		auto it = receiver->lastJoystickState.find(id);
		if (it == receiver->lastJoystickState.end()) return false;
		if (id < 0 || id >= receiver->joysticks.size()) return false;

		u32 btnCount = receiver->joysticks[id].Buttons;
		if (buttonID >= btnCount || buttonID >= 32u) return false;

		return (it->second.ButtonStates & (1u << buttonID)) != 0;
	}

	float getJoystickAxis(int id = 0, int axis = 0) {
		auto it = receiver->lastJoystickState.find(id);
		if (it == receiver->lastJoystickState.end()) return 0.0f;
		if (id < 0 || id >= receiver->joysticks.size()) return 0.0f;

		const irr::u32 axisCount = receiver->joysticks[id].Axes;
		if (axis < 0 || (irr::u32)axis >= axisCount) return 0.0f;

		int val = (int)it->second.Axis[axis];
		if (val >= -1.0 * 1000 && val <= 1000) return 0.0f; // Deadzone check

		return max(min(val / 32767.0f, 1.0), -1.0);
	}

	int getJoystickPadDirection(int id = 0) {
		auto it = receiver->lastJoystickState.find(id);
		if (it == receiver->lastJoystickState.end()) return -1;
		if (id < 0 || id >= receiver->joysticks.size()) return -1;

		if (it->second.POV == 0xFFFF) return -1;
		return (((int)it->second.POV + 2250) / 4500) % 8;
	}

	void checkForJoysticks() {
		if (!device) return;
		
		device->activateJoysticks(receiver->joysticks);
		for (auto it = receiver->lastJoystickState.begin(); it != receiver->lastJoystickState.end(); ) {
			const int32_t id = it->second.Joystick;
			dConsole.sendMsg(std::to_string(id).c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
			if (id < 0 || id + 1 >= receiver->joysticks.size()) {
				Events::Input::OnJoystickDisconnect.get()->engineRun(id);
				it = receiver->lastJoystickState.erase(it);
			} else
				++it;
		}
	}
}

void bindInput() {
	sol::table input = lua->create_named_table("Input");

	input["IsKeyDown"] = &Bind::isKeyPressed;
	input["GetMouseState"] = &Bind::getMouseState;
	input["IsMouseDown"] = &Bind::getMouseDown;
	input["SetMouseVisible"] = &Bind::showCursor;
	input["SetMousePosition"] = &Bind::setCursorPosition;
	input["GetMousePosition"] = &Bind::getCursorPosition;
	input["GetMouseDelta"] = &Bind::getMouseDelta;
	input["GetMouseWheelDelta"] = &Bind::getMouseWheelDelta;

	input["GetJoystickState"] = &Bind::getJoystickState;
	input["IsJoystickConnected"] = &Bind::isJoystickConnected;
	input["IsJoystickButtonPressed"] = &Bind::isJoystickButtonPressed;
	input["GetJoystickAxis"] = &Bind::getJoystickAxis;
	input["GetJoystickDPad"] = &Bind::getJoystickPadDirection;
	input["UpdateJoysticks"] = &Bind::checkForJoysticks;
}