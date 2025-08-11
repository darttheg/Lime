#pragma once

#include <irrlicht.h>
#include <sol/sol.hpp>
#include "LuaLime.h"
#include "IrrManagers.h"

class Compatible2D {
public:
    virtual ~Compatible2D() = default;

    irr::gui::IGUIButton* button = nullptr;
    bool clickable = false;
    sol::function onClick;
    sol::function onHover;

    virtual irr::gui::IGUIElement* getNode() const = 0;
    void setParent(sol::optional<Compatible2D*> parent);
    bool getClickable();
    bool getHovered();
    void setHovered();
    void setClickable(sol::function f);
    void setHover(sol::function hov);
    bool getPressed();
    void updateButton();
};

void bindCompatible2D();
