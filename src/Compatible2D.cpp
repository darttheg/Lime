#include "Compatible2D.h"

void Compatible2D::setParent(sol::optional<Compatible2D*> parent) {
	irr::gui::IGUIElement* node = getNode();
	if (!node || !(*parent)) return;

	// Add child etc.
	(*parent)->getNode()->addChild(node);
}

void Compatible2D::setDoInputEvents(bool enable) {
	if (getNode())
		doEvents = enable;
}

void bindCompatible2D() {
	sol::usertype<Compatible2D> bindType = lua->new_usertype<Compatible2D>("Compatible2D",	
		"hovered", &Compatible2D::hovered,
		"pressed", &Compatible2D::pressed
	);

	bindType["setParent"] = &Compatible2D::setParent;
	bindType["setDoInputEvents"] = &Compatible2D::setDoInputEvents;
}