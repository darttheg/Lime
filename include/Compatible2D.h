#pragma once

#include <irrlicht.h>
#include <sol/sol.hpp>
#include "LuaLime.h"
#include "IrrManagers.h"

#include "Event.h"
#include "Vector4D.h"
#include <functional>

class Compatible2D {
public:
    virtual ~Compatible2D() = default;

    virtual irr::gui::IGUIElement* getNode() const = 0;
    void setParent(sol::optional<Compatible2D*> parent);
    Vector2D getPosition();
    void setPosition(const Vector2D& pos);
    Vector2D getPositionProportional();
    void setPositionProportional(const Vector2D& pos);
    bool getVisible();
    void setVisible(bool v);
    void bringToFront();
    void sendToBack();
    Vector2D getSize();
    void setSize(const Vector2D& size);
    void setMinMaxDimensions(const Vector2D& dimMin, const Vector2D& dimMax);

    std::string getToolTip();
    void setToolTip(std::string tip);
    bool getPressed();
    bool getHovered();

    bool inReceiverStack = false;

    // Clickable/Hover
    bool doEvents = false;
    void setDoInputEvents(bool enable); // Do we even process this object for these special events?
    irr::gui::IGUIButton* button = nullptr;
    void initButton();

    void removeEventCallbacks();
    void clean();

    bool hovered = false;
    bool pressed = false; // Can be checked if held down

    std::shared_ptr<Event> onPressed = std::make_shared<Event>(); // Called when clicked OR click is forced (controller)
    std::shared_ptr<Event> onHover = std::make_shared<Event>(); // Repeatedly calls while mouse is over OR hover is forced (controller)

    std::shared_ptr<Event> getHoverEvent() { return onHover; }
    std::shared_ptr<Event> getPressedEvent() { return onPressed; }
    void setHoverEvent(std::shared_ptr<Event> e) { onHover = std::move(e); }
    void setPressedEvent(std::shared_ptr<Event> e) { onPressed = std::move(e); }
};

void bindCompatible2D();
