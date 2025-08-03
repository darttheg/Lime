#include "Empty.h"

Empty::Empty() {
	emp = smgr->addEmptySceneNode();
}

Empty::Empty(const Vector3D& pos) : Empty() {
	emp->setPosition(irr::core::vector3df(pos.x, pos.y, pos.z));
}

Empty::Empty(const Vector3D& pos, const Vector3D& rot) : Empty(pos) {
	emp->setRotation(irr::core::vector3df(rot.x, rot.y, rot.z));
}

Empty::Empty(const Vector3D& pos, const Vector3D& rot, const Vector3D& scale) : Empty(pos, rot) {
	emp->setScale(irr::core::vector3df(scale.x, scale.y, scale.z));
}

bool Empty::getVisibility() const {
	if (!emp)
		return false;
	return emp->isVisible();
}

void Empty::setVisibility(bool visible) {
	emp->setVisible(visible);
}

Vector3D Empty::getPosition() {
	if (!emp)
		return Vector3D();
	return Vector3D(emp->getPosition().X, emp->getPosition().Y, emp->getPosition().Z);
}

void Empty::setPosition(const Vector3D& pos) {
	if (!emp)
		return;
	emp->setPosition(irr::core::vector3df(pos.x, pos.y, pos.z));
}

Vector3D Empty::getRotation() {
	if (!emp)
		return Vector3D();
	return Vector3D(emp->getRotation().X, emp->getRotation().Y, emp->getRotation().Z);
}

void Empty::setRotation(const Vector3D& rot) {
	if (!emp)
		return;
	emp->setRotation(irr::core::vector3df(rot.x, rot.y, rot.z));
}

Vector3D Empty::getScale() {
	if (!emp)
		return Vector3D();
	return Vector3D(emp->getScale().X, emp->getScale().Y, emp->getScale().Z);
}

void Empty::setScale(const Vector3D& scale) {
	if (!emp)
		return;
	emp->setScale(irr::core::vector3df(scale.x, scale.y, scale.z));
}

bool Empty::getDebug() {
	return emp ? d != nullptr : false;
}

void Empty::setDebug(bool visible) {
	if (emp) {
		if (visible && !d)
			d = new DebugSceneNode(emp, smgr, 0, DebugType::EMPTY);
		else if (d)
			d->remove();
	}
}

sol::table Empty::getBoundingBox() {
	sol::table result = lua->create_table();
	result["min"] = Vector3D();
	result["max"] = Vector3D();

	if (emp) {
		core::aabbox3d<f32> bb = emp->getTransformedBoundingBox();
		result["min"] = Vector3D(bb.MinEdge.X, bb.MinEdge.Y, bb.MinEdge.Z);
		result["max"] = Vector3D(bb.MaxEdge.X, bb.MaxEdge.Y, bb.MaxEdge.Z);
	}

	return result;
}

void Empty::destroy() {
	if (emp)
		emp->remove();
}

void bindEmpty() {
	sol::usertype<Empty> bind_type = lua->new_usertype<Empty>("Empty",
		sol::constructors<Empty(), Empty(const Vector3D & pos), Empty(const Vector3D & pos, const Vector3D & rot), Empty(const Vector3D & pos, const Vector3D & rot, const Vector3D & scale)>(),

		sol::base_classes, sol::bases<Compatible3D>(),

		"visible", sol::property(&Empty::getVisibility, &Empty::setVisibility),
		"position", sol::property(&Empty::getPosition, &Empty::setPosition),
		"rotation", sol::property(&Empty::getRotation, &Empty::setRotation),
		"scale", sol::property(&Empty::getScale, &Empty::setScale),
		"debug", sol::property(&Empty::getDebug, &Empty::setDebug)
	);

	bind_type["destroy"] = &Empty::destroy;
	bind_type["getBoundingBox"] = &Empty::getBoundingBox;
}