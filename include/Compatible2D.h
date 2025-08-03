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

    void setParent(sol::optional<Compatible2D*> parent) {
        irr::gui::IGUIElement* node = getNode();
        if (!node || !(*parent)) return;

        // add child etc.
        (*parent)->getNode()->addChild(node);
    }

	bool getClickable() {
		return getNode() ? button != nullptr : false;
	}

	bool getHovered() {
		if (!getNode())
			return false;

		irr::core::position2di mousePos = device->getCursorControl()->getPosition();
		irr::core::recti rect(
			getNode()->getAbsoluteClippingRect().UpperLeftCorner,
			getNode()->getAbsoluteClippingRect().LowerRightCorner
		);
		return rect.isPointInside(mousePos);
	}

	void setHovered() {

	}

	void setClickable(sol::function f) {
		if (!getNode())
			return;
		if (f && !button) {
			clickable = true;
			updateButton();
			receiver->buttonCallbackClick.push_back(ButtonCallbackPairClick(button, f));
		}
		else if (!f && button) {
			receiver->removeImg(button);
			button->remove();
			clickable = false;
		}
	}

	void setHover(sol::function hov) {
		if (!getNode()) return;

		if (!clickable) {
			bool cl = clickable;
			clickable = true;
			updateButton();
			clickable = cl;
		}

		receiver->buttonCallbackHover.push_back(ButtonCallbackPairHover(button, hov));
	}

	bool getPressed() {
		if (!getNode()) return false;
		return button ? button->isPressed() : false;
	}

	void updateButton() {
		if (!clickable)
			return;

		irr::core::recti r = getNode()->getRelativePosition();
		if (button)
			receiver->removeImg(button);
		button = guienv->addButton(irr::core::recti(0, 0, r.LowerRightCorner.X - r.UpperLeftCorner.X, r.LowerRightCorner.Y - r.UpperLeftCorner.Y), getNode());
		button->setDrawBorder(false);
		button->setUseAlphaChannel(true);
	}
};

inline void bindCompatible2D() {
    sol::usertype<Compatible2D> bind_type = lua->new_usertype<Compatible2D>("Compatible2D",
		"hovered", sol::property(&Compatible2D::getHovered, &Compatible2D::setHovered),
		"pressed", sol::property(&Compatible2D::getPressed, &Compatible2D::setHovered));

    bind_type["setParent"] = &Compatible2D::setParent;
	bind_type["fireOnClick"] = &Compatible2D::setClickable;
	bind_type["fireOnHover"] = &Compatible2D::setHover;
}
