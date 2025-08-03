#include "LimeReceiver.h"

void LimeReceiver::removeImg(irr::gui::IGUIButton* b) {
    buttonCallbackClick.erase(
        std::remove_if(
            buttonCallbackClick.begin(),
            buttonCallbackClick.end(),
            [b](const ButtonCallbackPairClick& pair) {
                return pair.button == b;
            }
        ),
        buttonCallbackClick.end()
    );

    buttonCallbackHover.erase(
        std::remove_if(
            buttonCallbackHover.begin(),
            buttonCallbackHover.end(),
            [b](const ButtonCallbackPairHover& pair) {
                return pair.button == b;
            }
        ),
        buttonCallbackHover.end()
    );
}

LimeReceiver::LimeReceiver()
{
    keys.fill(false);
    keysRepeat.fill(false);
}

bool LimeReceiver::OnEvent(const SEvent& event)
{
    // Handle keyboard input events
    if (event.EventType == EET_KEY_INPUT_EVENT)
    {
        keys[event.KeyInput.Key] = event.KeyInput.PressedDown;

        if (event.KeyInput.PressedDown) {
            if (!keysRepeat[event.KeyInput.Key]) {
                callLuaFunction("Input", "OnKeyPressed", static_cast<irr::EKEY_CODE>(event.KeyInput.Key));
                keysRepeat[event.KeyInput.Key] = true;
            }
        }
        else {
            callLuaFunction("Input", "OnKeyReleased", static_cast<irr::EKEY_CODE>(event.KeyInput.Key));
            keysRepeat[event.KeyInput.Key] = false;
        }
    }

    // Handle mouse input events
    if (event.EventType == EET_MOUSE_INPUT_EVENT)
    {
        switch (event.MouseInput.Event)
        {
        case EMIE_LMOUSE_PRESSED_DOWN:
            MouseState.LeftButtonDown = true;
            callLuaFunction("Input", "OnLeftMouseClick");
            break;

        case EMIE_LMOUSE_LEFT_UP:
            MouseState.LeftButtonDown = false;
            break;

        case EMIE_RMOUSE_PRESSED_DOWN:
            MouseState.RightButtonDown = true;
            callLuaFunction("Input", "OnRightMouseClick");
            break;

        case EMIE_RMOUSE_LEFT_UP:
            MouseState.RightButtonDown = false;
            break;

        case EMIE_MMOUSE_PRESSED_DOWN:
            MouseState.MiddleButtonDown = true;
            callLuaFunction("Input", "OnMiddleMouseClick");
            break;

        case EMIE_MMOUSE_LEFT_UP:
            MouseState.MiddleButtonDown = false;
            break;

        case EMIE_MOUSE_MOVED:
            MouseState.Position.X = event.MouseInput.X;
            MouseState.Position.Y = event.MouseInput.Y;
            callLuaFunction("Input", "OnMouseMove", Vector2D(MouseState.Position.X, MouseState.Position.Y));
            break;

        case EMIE_MOUSE_WHEEL:
            MouseState.WheelDelta = event.MouseInput.Wheel;
            callLuaFunction("Input", "OnMouseScroll", MouseState.WheelDelta);
            break;

        default:
            break;
        }
    }

    if (event.EventType == EET_JOYSTICK_INPUT_EVENT)
    {
        JoystickState = event.JoystickEvent;

        for (u32 i = 0; i < SEvent::SJoystickEvent::NUMBER_OF_AXES; ++i)
        {
            ControllerState.Axis[i] = JoystickState.Axis[i];
        }

        ControllerState.Buttons = JoystickState.ButtonStates;
    }

    if (event.GUIEvent.EventType == irr::gui::EGET_ELEMENT_FOCUSED || event.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
        irr::gui::IGUIButton* button = static_cast<irr::gui::IGUIButton*>(event.GUIEvent.Caller);
        for (const auto& pair : buttonCallbackClick) {
            if (pair.button == button) {
                if (pair.button == lastFocused) return true;
                lastFocused = pair.button;
                if (pair.callback) pair.callback(event.GUIEvent.EventType == irr::gui::EGET_ELEMENT_FOCUSED);
                return true;
            }
        }
    }

    if (event.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED || event.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT) {
        irr::gui::IGUIButton* clickedButton = static_cast<irr::gui::IGUIButton*>(event.GUIEvent.Caller);
        for (const auto& pair : buttonCallbackHover) {
            if (pair.button == clickedButton) {
                if (pair.hover) pair.hover(event.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED);
                return true;
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

sol::table LimeReceiver::getControllerState() const
{
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
}

// Check if a key is currently pressed
bool LimeReceiver::isKeyDown(irr::EKEY_CODE keyCode) const
{
    return keys[keyCode];
}