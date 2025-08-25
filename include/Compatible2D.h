#pragma once

#include <irrlicht.h>
#include <sol/sol.hpp>
#include "LuaLime.h"
#include "IrrManagers.h"

#include "Event.h"

class Compatible2D {
public:
    virtual ~Compatible2D() = default;

    virtual irr::gui::IGUIElement* getNode() const = 0;
    void setParent(sol::optional<Compatible2D*> parent);

    // Clickable/Hover
    bool doEvents = false;
    void setDoInputEvents(bool enable); // Do we even process this object for these special events?

    bool hovered = false;
    bool pressed = false; // Can be checked if held down

    std::shared_ptr<Event> onPressed; // Called when clicked OR click is forced (controller)
    std::shared_ptr<Event> onHover; // Repeatedly calls while mouse is over OR hover is forced (controller)
};

void bindCompatible2D();
