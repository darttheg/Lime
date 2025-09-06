#include "Compatible2D.h"

void Compatible2D::setParent(sol::optional<Compatible2D*> parent) {
	irr::gui::IGUIElement* node = getNode();
	if (!node || !(*parent)) return;

	// Add child etc.
	(*parent)->getNode()->addChild(node);
}

void Compatible2D::setDoInputEvents(bool enable) {
	if (!getNode()) return;

	/*
	When a 2D element gets input events enabled, it creates a hidden button.
	When the input events are disabled, the callback is dropped but the button remains, but disabled.
	The user can still call its associated events when the button is disabled.
	
	All 2D objects would be instantiated with buttons but that would probably be bad.
	*/

	bool old = doEvents;
	doEvents = enable;

	if (doEvents != old) {
		if (enable)
			initButton();
		else
			removeEventCallbacks();

		if (button)
			button->setEnabled(enable);
	}
}

void Compatible2D::initButton() { // Called on making clickable
	if (!getNode()) return;

	irr::core::recti rect = getNode()->getRelativePosition();
	rect = irr::core::recti(0, 0, rect.LowerRightCorner.X - rect.UpperLeftCorner.X, rect.LowerRightCorner.Y - rect.UpperLeftCorner.Y);

	if (!button) {
		button = guienv->addButton(rect, getNode());
		// button->setVisible(false); Makes button not do events
		button->setDrawBorder(false);
	}
	else {
		button->setRelativePosition(rect);
	}

	if (!inReceiverStack) {
		receiver->guiElements[button] = ButtonEvents(onHover, onPressed);
		inReceiverStack = true;
	}
}

void Compatible2D::removeEventCallbacks() {
	// Called on Compatible2D::destroy
	receiver->guiElements.erase(button);
	inReceiverStack = false;
}

void Compatible2D::clean() {
	if (button) {
		removeEventCallbacks();
		button->drop();
	}
}

Vector2D Compatible2D::getPosition() {
	if (getNode())
		return Vector2D(getNode()->getRelativePosition().UpperLeftCorner.X, getNode()->getRelativePosition().UpperLeftCorner.Y);
	return Vector2D();
}

void Compatible2D::setPosition(const Vector2D& pos) {
	if (getNode())
		getNode()->setRelativePosition(irr::core::position2di(pos.x, pos.y));
}

bool Compatible2D::getVisible() {
	return getNode() ? getNode()->isVisible() : false;
}

void Compatible2D::setVisible(bool vis) {
	if (getNode())
		getNode()->setVisible(vis);
}

void Compatible2D::bringToFront() {
	if (getNode())
		getNode()->bringToFront(getNode());
}

void Compatible2D::sendToBack() {
	if (getNode())
		getNode()->sendToBack(getNode());
}

Vector2D Compatible2D::getSize() {
	if (getNode()) {
		irr::core::recti r = getNode()->getRelativePosition();
		return Vector2D(r.LowerRightCorner.X - r.UpperLeftCorner.X, r.LowerRightCorner.Y - r.UpperLeftCorner.Y);
	}
	return Vector2D();
}

void Compatible2D::setSize(const Vector2D& size) {
	if (getNode()) {
		irr::core::recti r = getNode()->getRelativePosition();
		r.LowerRightCorner.X = r.UpperLeftCorner.X + size.x;
		r.LowerRightCorner.Y = r.UpperLeftCorner.Y + size.y;
		getNode()->setRelativePosition(r);

		if (button)
			button->setRelativePosition(r);
	}
}

void Compatible2D::setMinMaxDimensions(const Vector2D& dimMin, const Vector2D& dimMax) {
	if (getNode()) {
		getNode()->setMinSize(irr::core::dimension2du(dimMin.x, dimMin.y));
		getNode()->setMaxSize(irr::core::dimension2du(dimMax.x, dimMax.y));
	}
}

bool Compatible2D::getHovered() {
	if (!button || !button->isEnabled())
		return false;
	
	return button->isPointInside(irr::core::vector2di(receiver->mouseX, receiver->mouseY));
}

bool Compatible2D::getPressed() {
	if (!button || !button->isEnabled())
		return false;

	return button->isPressed();
}

std::string Compatible2D::getToolTip() {
	if (!button || !button->isEnabled())
		return "";

	irr::core::stringw og = button->getToolTipText();
	std::string out(og.c_str(), og.c_str() + og.size());

	return out;
}

void Compatible2D::setToolTip(std::string tip) {
	if (!button || !button->isEnabled())
		return;

	std::wstring out(tip.begin(), tip.end());
	button->setToolTipText(out.c_str());
}

#include "Proxy.h"
void bindCompatible2D() {
	sol::usertype<Compatible2D> bindType = lua->new_usertype<Compatible2D>("Compatible2D",

		"position", sol::property(
			[](Compatible2D& c) { return Vector2DProxy{ [&] { return c.getPosition(); }, [&](auto v) { c.setPosition(v); } }; },
			[](Compatible2D& c, const Vector2D& v) { c.setPosition(v); }
		),
		"size", sol::property(
			[](Compatible2D& c) { return Vector2DProxy{ [&] { return c.getSize(); }, [&](auto v) { c.setSize(v); } }; },
			[](Compatible2D& c, const Vector2D& v) { c.setSize(v); }
		),

		"visible", sol::property(&Compatible2D::getVisible, &Compatible2D::setVisible),
		"toolTip", sol::property(&Compatible2D::getToolTip, &Compatible2D::setToolTip),

		"hovered", &Compatible2D::hovered,
		"pressed", &Compatible2D::pressed
	);

	bindType["OnHover"] = sol::property([](Compatible2D& self) { return self.getHoverEvent(); });
	bindType["OnPressed"] = sol::property([](Compatible2D& self) { return self.getPressedEvent(); });

	bindType["setParent"] = &Compatible2D::setParent;
	bindType["setInteractable"] = &Compatible2D::setDoInputEvents;
	bindType["toFront"] = &Compatible2D::bringToFront;
	bindType["toBack"] = &Compatible2D::sendToBack;
	bindType["setMinMaxDimensions"] = &Compatible2D::setMinMaxDimensions;

	bindType["isHovered"] = &Compatible2D::getHovered;
	bindType["isPressed"] = &Compatible2D::getPressed;
}