#include "Empty.h"

Empty::Empty() {
	emp = smgr->addEmptySceneNode();
}

Empty::Empty(const Vector3D& pos) : Empty() {
	setPosition(pos);
}

Empty::Empty(const Vector3D& pos, const Vector3D& rot) : Empty(pos) {
	setRotation(rot);
}

Empty::Empty(const Vector3D& pos, const Vector3D& rot, const Vector3D& scale) : Empty(pos, rot) {
	setScale(scale);
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
	if (d) d->remove();
	if (emp) {
		destroyEntry();
		smgr->addToDeletionQueue(emp); // Replace all destroys with this!
	}
}

void bindEmpty() {
	sol::usertype<Empty> bindType = lua->new_usertype<Empty>("Empty",
		sol::constructors<Empty(), Empty(const Vector3D & pos), Empty(const Vector3D & pos, const Vector3D & rot), Empty(const Vector3D & pos, const Vector3D & rot, const Vector3D & scale)>(),

		sol::base_classes, sol::bases<Compatible3D>(),
		sol::meta_function::type, [](const Empty&) { return "Empty"; },

		"debug", sol::property(&Empty::getDebug, &Empty::setDebug)
	);

	bindType["getBoundingBox"] = &Empty::getBoundingBox;
}