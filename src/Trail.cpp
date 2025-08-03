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
		t->remove();
		empty->remove();
	}
}

bool Trail::getVisibility() const {
	return t ? t->isVisible() : false;
}

void Trail::setVisibility(bool visible) {
	t->setVisible(visible);
}

Vector3D Trail::getPosition() {
	if (!t)
		return Vector3D();
	return Vector3D(empty->getPosition().X, empty->getPosition().Y, empty->getPosition().Z);
}

void Trail::setPosition(const Vector3D& pos) {
	if (!t)
		return;
	empty->setPosition(irr::core::vector3df(pos.x, pos.y, pos.z));
}

Vector3D Trail::getRotation() {
	if (!t)
		return Vector3D();
	return Vector3D(empty->getRotation().X, empty->getRotation().Y, empty->getRotation().Z);
}

void Trail::setRotation(const Vector3D& rot) {
	if (!t)
		return;
	empty->setRotation(irr::core::vector3df(rot.x, rot.y, rot.z));
}

Vector3D Trail::getScale() {
	if (!t)
		return Vector3D();
	return Vector3D(empty->getScale().X, empty->getScale().Y, empty->getScale().Z);
}

void Trail::setScale(const Vector3D& scale) {
	if (!t)
		return;
	empty->setScale(irr::core::vector3df(scale.x, scale.y, scale.z));
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

void Trail::exclude() {
	if (t)
		effects->excludeNodeFromLightingCalculations(t);
}

int Trail::getShadows() {
	return t ? shadow : 0;
}

void Trail::setShadows(int i) {
	if (t) {
		shadow = i;
		E_SHADOW_MODE mode = (E_SHADOW_MODE)i;
		if (!hadShadow && (mode == E_SHADOW_MODE::ESM_BOTH || mode == E_SHADOW_MODE::ESM_CAST)) {
			effects->addShadowToNode(t, irrHandler->defaultShadowFiltering, (E_SHADOW_MODE)shadow);
			hadShadow = true;
		}
		else if (hadShadow) {
			effects->removeShadowFromNode(t);
			hadShadow = false;
		}
	}
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
	sol::usertype<Trail> bind_type = lua->new_usertype<Trail>("Trail",
		sol::constructors<Trail()>(),

		sol::base_classes, sol::bases<Compatible3D>(),

		"visible", sol::property(&Trail::getVisibility, &Trail::setVisibility),
		"position", sol::property(&Trail::getPosition, &Trail::setPosition),
		"rotation", sol::property(&Trail::getRotation, &Trail::setRotation),
		"scale", sol::property(&Trail::getScale, &Trail::setScale),
		"debug", sol::property(&Trail::getDebug, &Trail::setDebug),
		"shadows", sol::property(&Trail::getShadows, &Trail::setShadows),
		"height", sol::property(&Trail::getWidth, &Trail::setWidth),
		"wind", sol::property(&Trail::getWind, &Trail::setWind),
		"segments", sol::property(&Trail::getSegments, &Trail::setSegments),
		"alignment", sol::property(&Trail::getAlignmentMode, &Trail::setAlignmentMode),
		"segmentLength", sol::property(&Trail::getFixedSize, &Trail::setFixedSize)
	);

	bind_type["destroy"] = &Trail::destroy;
	bind_type["setParent"] = &Trail::setParent;
	bind_type["loadMaterial"] = &Trail::loadMaterial;
	bind_type["ignoreLighting"] = &Trail::exclude;
	bind_type["updateNormals"] = &Trail::setUpdateNormals;
}