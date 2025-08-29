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

    // Handle controller event
    if (event.EventType == EET_JOYSTICK_INPUT_EVENT) {
        JoystickState = event.JoystickEvent;

        for (u32 i = 0; i < SEvent::SJoystickEvent::NUMBER_OF_AXES; ++i)
        {
            ControllerState.Axis[i] = JoystickState.Axis[i];
        }

        ControllerState.Buttons = JoystickState.ButtonStates;
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

sol::table LimeReceiver::getJoystickState(int id) const
{
    /*
    sol::table table = lua->create_table();

    sol::table axisTable = lua->create_table();
    for (int i = 0; i < SEvent::SJoystickEvent::NUMBER_OF_AXES; i++) {
        axisTable[i + 1] = ControllerState.Axis[i] / 32767.f;
    }

    sol::table buttonTable = lua->create_table();
    for (int i = 0; i < 32; ++i) {
        buttonTable[i + 1] = ControllerState.isButtonPressed(i);
    }

    table["axis"] = axisTable;
    table["buttons"] = buttonTable;
    return table;
    */
}

// Check if a key is currently pressed
bool LimeReceiver::isKeyDown(irr::EKEY_CODE keyCode) const
{
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
        deltaX = mouseX - lastMouseX;
        deltaY = mouseY - lastMouseY;
    }
}

// Called after Lua is run
void LimeReceiver::updateLastMouse() {
    // set mouse x in warden
    lastMouseX = mouseX;
    lastMouseY = mouseY;
}