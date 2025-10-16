#include "Trail.h"

Trail::Trail() {
	empty = smgr->addEmptySceneNode();
	trailWidth = 10.0f;
	segments = 64;
	mode = 3;

	make();
}

void Trail::make() {
	irr::video::SMaterial cur;
	cur.Lighting = false;
	if (t) {
		cur = t->getMaterial(0);
		t->remove();
	}

	t = new ghostTrailSceneNode(empty, smgr, -1, trailWidth, segments);
	t->getMaterial(0) = cur;
	setAlignmentMode(mode);
}

void Trail::destroy() {
	if (t) {
		destroyEntry();
		smgr->addToDeletionQueue(t);
		smgr->addToDeletionQueue(empty);
	}
}

float Trail::getWidth() {
	return t ? trailWidth : 0.0f;
}

void Trail::setWidth(float w) {
	if (!t)
		return;
	t->setTrailWidth(-w);
	trailWidth = w;
}

int Trail::getSegments() {
	return t ? segments : 0;
}

void Trail::setSegments(int s) {
	if (!t)
		return;

	segments = s;
	make();
}

Vector3D Trail::getWind() {
	return t ? wind : Vector3D();
}

void Trail::setWind(const Vector3D& w) {
	if (!t)
		return;

	wind = w;
	t->setWind(irr::core::vector3df(wind.x, wind.y, wind.z));
}

bool Trail::getDebug() {
	if (t)
		return t->isDebugDataVisible();
	return false;
}

void Trail::setDebug(bool visible) {
	if (t) {
		if (visible)
			t->setDebugDataVisible(irr::scene::EDS_FULL);
		else
			t->setDebugDataVisible(irr::scene::EDS_OFF);
	}
}

bool Trail::loadMaterial(const Material& material) {
	if (!t) return false;

	t->getMaterial(0) = material.mat;
	t->getMaterial(0).Lighting = false;

	return true;
}

void Trail::setUpdateNormals(bool enable) {
	if (t)
		t->setUpdateNormals(enable);
}

int Trail::getAlignmentMode() {
	return t ? mode : 0;
}

void Trail::setAlignmentMode(int m) {
	if (t) {
		t->setAlignmentMode((ghostTrailSceneNode::E_ALIGNMENT_MODE)m);
		mode = m;
	}
}

float Trail::getFixedSize() {
	return t ? t->getFixedSize() : 0.0f;
}

void Trail::setFixedSize(float s) {
	if (t)
		t->setFixedSize(s);
}


void bindTrail() {
	sol::usertype<Trail> bindType = lua->new_usertype<Trail>("Trail",
		sol::constructors<Trail()>(),

		sol::base_classes, sol::bases<Compatible3D>(),

		"debug", sol::property(&Trail::getDebug, &Trail::setDebug),
		"height", sol::property(&Trail::getWidth, &Trail::setWidth),

		"wind", sol::property(
			[](Trail& c) { return Vector3D{ [&] { return c.getWind(); }, [&](auto v) { c.setWind(v); } }; },
			[](Trail& c, const Vector3D& v) { c.setWind(v); }
		),

		"segments", sol::property(&Trail::getSegments, &Trail::setSegments),
		"alignment", sol::property(&Trail::getAlignmentMode, &Trail::setAlignmentMode),
		"segmentLength", sol::property(&Trail::getFixedSize, &Trail::setFixedSize)
	);

	bindType["setParent"] = &Trail::setParent;
	bindType["loadMaterial"] = &Trail::loadMaterial;
	bindType["updateNormals"] = &Trail::setUpdateNormals;
}