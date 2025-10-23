#include "LimeReceiver.h"

#include "LimeEvents.h"

using namespace irr;
using namespace gui;

LimeReceiver::LimeReceiver()
{
    keys.fill(false);
    keysRepeat.fill(false);
}

bool LimeReceiver::OnEvent(const SEvent& event)
{
    // Handle keyboard input events
    if (event.EventType == EET_KEY_INPUT_EVENT) {
        keys[event.KeyInput.Key] = event.KeyInput.PressedDown;

        if (event.KeyInput.PressedDown) {
            if (!keysRepeat[event.KeyInput.Key]) {
                Events::Input::OnKeyPressed.get()->engineRun(static_cast<irr::EKEY_CODE>(event.KeyInput.Key));
                // callLuaFunction("Input", "OnKeyPressed", static_cast<irr::EKEY_CODE>(event.KeyInput.Key));
                keysRepeat[event.KeyInput.Key] = true;
            }
        }
        else {
            Events::Input::OnKeyReleased.get()->engineRun(static_cast<irr::EKEY_CODE>(event.KeyInput.Key));
            // callLuaFunction("Input", "OnKeyReleased", static_cast<irr::EKEY_CODE>(event.KeyInput.Key));
            keysRepeat[event.KeyInput.Key] = false;
        }
    }

    // Handle mouse input events
    if (event.EventType == EET_MOUSE_INPUT_EVENT) {
        switch (event.MouseInput.Event)
        {
        case EMIE_LMOUSE_PRESSED_DOWN:
            MouseState.LeftButtonDown = true;
            Events::Input::OnMouseClick.get()->engineRun(0);
            // callLuaFunction("Input", "OnLeftMouseClick");
            break;

        case EMIE_LMOUSE_LEFT_UP:
            MouseState.LeftButtonDown = false;
            break;

        case EMIE_RMOUSE_PRESSED_DOWN:
            MouseState.RightButtonDown = true;
            Events::Input::OnMouseClick.get()->engineRun(2);
            // callLuaFunction("Input", "OnRightMouseClick");
            break;

        case EMIE_RMOUSE_LEFT_UP:
            MouseState.RightButtonDown = false;
            break;

        case EMIE_MMOUSE_PRESSED_DOWN:
            MouseState.MiddleButtonDown = true;
            Events::Input::OnMouseClick.get()->engineRun(1);
            // callLuaFunction("Input", "OnMiddleMouseClick");
            break;

        case EMIE_MMOUSE_LEFT_UP:
            MouseState.MiddleButtonDown = false;
            break;

        case EMIE_MOUSE_MOVED:
            MouseState.Position.X = event.MouseInput.X;
            MouseState.Position.Y = event.MouseInput.Y;
            Events::Input::OnMouseMove.get()->engineRun(Vector2D(MouseState.Position.X, MouseState.Position.Y));
            
            // callLuaFunction("Input", "OnMouseMove", Vector2D(MouseState.Position.X, MouseState.Position.Y));
            break;

        case EMIE_MOUSE_WHEEL:
            MouseState.WheelDelta = event.MouseInput.Wheel;
            Events::Input::OnMouseScroll.get()->engineRun(MouseState.WheelDelta);
            
            // callLuaFunction("Input", "OnMouseScroll", MouseState.WheelDelta);
            break;

        default:
            break;
        }
    }

    // Irrlicht sends joystick updates per frame
    if (event.EventType == EET_JOYSTICK_INPUT_EVENT) {
        bool callConnect = false;

        auto it = lastJoystickState.find(event.JoystickEvent.Joystick);
        if (it == lastJoystickState.end())
            callConnect = true;

        int32_t id = event.JoystickEvent.Joystick;

        // Handle button presses etc.
        if (!callConnect) {
            uint32_t btnCount = (id >= 0 && id < joysticks.size()) ? joysticks[id].Buttons : 32u;

            uint32_t prev = it->second.ButtonStates;
            uint32_t now = event.JoystickEvent.ButtonStates;

            uint32_t pressedMask = (~prev) & now;
            uint32_t releasedMask = prev & (~now);

            uint32_t limit = (btnCount >= 32) ? 32u : btnCount;
            for (uint32_t i = 0; i < limit; ++i) {
                uint32_t bit = 1u << i;
                if (pressedMask & bit) Events::Input::OnJoystickButtonPressed.get()->engineRun(id, i);
                if (releasedMask & bit) Events::Input::OnJoystickButtonReleased.get()->engineRun(id, i);
            }

            // Dpad
            if (event.JoystickEvent.POV != lastJoystickState[id].POV) {
                if (event.JoystickEvent.POV != 0xFFFF)
                    Events::Input::OnJoystickDPadDelta.get()->engineRun(id, ((event.JoystickEvent.POV + 2250) / 4500) % 8);
                else
                    Events::Input::OnJoystickDPadDelta.get()->engineRun(id, -1); 
            }
        }

        // Update it
        lastJoystickState[event.JoystickEvent.Joystick] = event.JoystickEvent;

        // Call the connect function so we can work with the event we just received.
        if (callConnect)
            Events::Input::OnJoystickConnect.get()->engineRun(id);
    }

    // Handle GUI event
    if (event.EventType == EET_GUI_EVENT) {
        auto it = guiElements.find(event.GUIEvent.Caller);
        if (it != guiElements.end()) {
            switch (event.GUIEvent.EventType) {
            case EGUI_EVENT_TYPE::EGET_ELEMENT_HOVERED:
                it->second.hoverEvent.get()->engineRun();
                break;
            case EGUI_EVENT_TYPE::EGET_BUTTON_CLICKED:
                it->second.pressedEvent.get()->engineRun();
                break;
            }
        }
    }

    return false;
}

sol::table LimeReceiver::getMouseState() const
{
    sol::table table = lua->create_table();
    table["position"] = Vector2D(MouseState.Position.X, MouseState.Position.Y);
    table["leftDown"] = MouseState.LeftButtonDown;
    table["rightDown"] = MouseState.RightButtonDown;
    table["middleDown"] = MouseState.MiddleButtonDown;
    table["wheelDelta"] = MouseState.WheelDelta;
    return table;
}

// Check if a key is currently pressed
bool LimeReceiver::isKeyPressed(irr::EKEY_CODE keyCode) const {
    return keys[keyCode];
}

/*
Mouse is moved
updateDeltaMouse
Lua calls
updateLastMouse
*/

// Called before Lua is run
void LimeReceiver::updateDeltaMouse(GLFWwindow* win) {
    if (skipDeltaOnResize)
        return;

    glfwGetCursorPos(win, &mouseX, &mouseY);

    if (firstMouse) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouse = false;
    } else {
        deltaX = std::round(mouseX - lastMouseX);
        deltaY = std::round(mouseY - lastMouseY);
    }
}

// Called after Lua is run
void LimeReceiver::updateLastMouse() {
    // set mouse x in warden
    lastMouseX = mouseX;
    lastMouseY = mouseY;
}