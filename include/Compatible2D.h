#pragma once

#include <irrlicht.h>
#include <sol/sol.hpp>
#include "LuaLime.h"
#include "IrrManagers.h"

class Compatible2D {
public:
    virtual ~Compatible2D() = default;

    virtual irr::gui::IGUIElement* getNode() const = 0;
    void setParent(sol::optional<Compatible2D*> parent);
};

void bindCompatible2D();
