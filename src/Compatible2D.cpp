#include "Compatible2D.h"

void Compatible2D::setParent(sol::optional<Compatible2D*> parent) {
	irr::gui::IGUIElement* node = getNode();
	if (!node || !(*parent)) return;

	// add child etc.
	(*parent)->getNode()->addChild(node);
}

void bindCompatible2D() {
	sol::usertype<Compatible2D> bind_type = lua->new_usertype<Compatible2D>("Compatible2D");

	bind_type["setParent"] = &Compatible2D::setParent;
}