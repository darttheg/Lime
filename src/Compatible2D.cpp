#include "Compatible2D.h"

void Compatible2D::setParent(sol::optional<Compatible2D*> parent) {
	irr::gui::IGUIElement* node = getNode();
	if (!node || !(*parent)) return;

	// Add child etc.
	(*parent)->getNode()->addChild(node);
}

void Compatible2D::setDoInputEvents(bool enable) {
	if (!getNode()) return;

	bool old = doEvents;
	doEvents = enable;

	if (doEvents != old) {
		if (enable)
			initButton();
		else {
			// TODO: Remove button from receiver callback

			button->drop();
		}
	}
}

void Compatible2D::initButton() { // Called on making clickable or resizing/new texture loaded
	if (!getNode()) return;

	if (button) button->drop();

	irr::core::recti rect = getNode()->getRelativePosition();
	rect = irr::core::recti(0, 0, rect.LowerRightCorner.X - rect.UpperLeftCorner.X, rect.LowerRightCorner.Y - rect.UpperLeftCorner.Y);

	button = guienv->addButton(rect, getNode());
	// button->setVisible(false); Commented for debug purposes
	// TODO: Add to GUI callbacks
}

void bindCompatible2D() {
	sol::usertype<Compatible2D> bindType = lua->new_usertype<Compatible2D>("Compatible2D",	
		"hovered", &Compatible2D::hovered,
		"pressed", &Compatible2D::pressed
	);

	bindType["setParent"] = &Compatible2D::setParent;
	bindType["setInteractable"] = &Compatible2D::setDoInputEvents;
}