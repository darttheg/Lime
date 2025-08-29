#include "FunctionsImports.h"

namespace Bind {
	sol::table getMouseState() {
		if (receiver) {
			return receiver->getMouseState();
		}
		return sol::table();
	}

	sol::table getJoystickState(int id) {
		if (receiver) {
			return receiver->getJoystickState(id);
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
			device->getCursorControl()->setPosition(irr::core::vector2di(pos.x, pos.y));
			receiver->mouseX = pos.x - 1;
			receiver->mouseY = pos.y - 1;
		}
	}
}

void bindInput() {
	sol::table input = lua->create_named_table("Input");

	input["IsKeyPressed"] = &Bind::isKeyDown;
	input["GetMouseState"] = &Bind::getMouseState;
	input["SetMouseVisible"] = &Bind::showCursor;
	input["SetMousePosition"] = &Bind::setCursorPosition;
	input["GetMousePosition"] = &Bind::getCursorPosition;
	input["GetMouseDelta"] = &Bind::getMouseDelta;
	// input["GetJoystickState"] = &Bind::getJoystickState;
	// input["IsJoystickConnected"] = &Bind::isJoystickConnected;
	// input["IsJoystickButtonPressed"] = &Bind::isJoystickButtonPressed;
}