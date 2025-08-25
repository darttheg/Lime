#pragma once
#include <irrlicht.h>
#include <sol/sol.hpp>
#include "LuaLime.h"
#include "Vector2D.h"
#include <Compatible2D.h>

using namespace irr;

class LimeReceiver : public IEventReceiver
{
public:
    struct SMouseState
    {
        core::position2di Position;
        bool LeftButtonDown;
        bool RightButtonDown;
        bool MiddleButtonDown;
        float WheelDelta;

        SMouseState()
            : LeftButtonDown(false), RightButtonDown(false), MiddleButtonDown(false), WheelDelta(0.0f)
        {}
    } MouseState;

    struct SControllerState
    {
        f32 Axis[SEvent::SJoystickEvent::NUMBER_OF_AXES];
        u32 Buttons;

        SControllerState()
        {
            std::fill(std::begin(Axis), std::end(Axis), 0.0f);
            Buttons = 0;
        }

        bool isButtonPressed(u32 buttonIndex) const
        {
            return Buttons & (1 << buttonIndex);
        }
    } ControllerState;

    // GUI Events
    std::unordered_map<irr::gui::IGUIButton*, Compatible2D*> guiElements;
    // On GUI event, if click or hover, see if caller is in this map. If so, call its events etc.

    LimeReceiver();

    virtual bool OnEvent(const SEvent& event) override;

    sol::table getMouseState() const;

    sol::table getControllerState() const; // Add specification for more than one controller

    // Check if a key is currently pressed
    bool isKeyDown(irr::EKEY_CODE keyCode) const;
    void updateDeltaMouse(GLFWwindow* win);
    void updateLastMouse();

    double mouseX, mouseY;
    double lastMouseX, lastMouseY;
    double deltaX, deltaY;
    bool firstMouse = true;
    bool skipDeltaOnResize = false;

private:
    std::array<bool, KEY_KEY_CODES_COUNT> keys;
    std::array<bool, KEY_KEY_CODES_COUNT> keysRepeat;
    SEvent::SJoystickEvent JoystickState;

    template<typename... Args>
    void callLuaFunction(const std::string& tableName, const std::string& functionName, Args&&... args)
    {
        sol::object obj = (*lua)[tableName][functionName];
        if (obj.get_type() == sol::type::function)
        {
            sol::protected_function func = obj.as<sol::protected_function>();
            sol::protected_function_result result = func(std::forward<Args>(args)...);
            if (!result.valid()) {
                sol::error err = result;
                dConsole.sendMsg(std::string(err.what()).c_str(), MESSAGE_TYPE::WARNING);
            } // Doesn't catch like SetWindowSize(x, y) instead of vector2D param
        }
        else {
            // Uncommenting this prints errors for things like Input.OnMouseMove etc.
            //std::string out = "Function " + tableName + "." + functionName + " not found or callable.";
            //dConsole.sendMsg(out.c_str(), MESSAGE_TYPE::WARNING);
        }
    }
};