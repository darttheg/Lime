#include "Light.h"

void Light::createBase() {
	target = smgr->addEmptySceneNode();
	holder = smgr->addEmptySceneNode();

	holder->addChild(target);
	target->setPosition(irr::core::vector3df(0, 0, 50));
}

Light::Light() : Light(Vector3D(0, 0, 0), Vector3D(0, 0, 0), Vector4D(255, 255, 255, 255), Vector2D(0.1, 50.0), 90, false) {
}
Light::Light(const Vector3D& pos, const Vector3D& rot, const Vector4D& col)
	: Light(pos, rot, col, Vector2D(0.1, 50.0), 90, false) {
}
Light::Light(const Vector3D& pos, const Vector3D& rot, const Vector4D& col, const Vector2D& viewPlanes, float fov, bool directional) {
	createBase();

	SColorf c = SColorf(col.x / 255.0, col.y / 255.0, col.z / 255.0, col.w / 255.0);
	holder->setPosition(vector3df(pos.x, pos.y, pos.z));

	index = irrHandler->lights;
	irrHandler->lights++;

	SShadowLight s = SShadowLight(irrHandler->defaultShadowResolution, holder->getPosition(), target->getAbsolutePosition(),
		c, viewPlanes.x, viewPlanes.y, fov * DEGTORAD, directional, index);
	effects->addShadowLight(s);

	updateTarget();
}

void Light::updateTarget() {
	holder->updateAbsolutePosition();
	target->updateAbsolutePosition();
	effects->getShadowLight(index).setTarget(target->getAbsolutePosition());

	if (d) {
		d->remove();
		d = new DebugSceneNode(holder, smgr, 0, DebugType::LIGHT);
	}
}

Vector3D Light::getPosition() {
	return true ? Vector3D(target->getPosition().X, target->getPosition().Y, target->getPosition().Z) : Vector3D();
}

void Light::setPosition(const Vector3D& pos) {
	if (true) {
		holder->setPosition(vector3df(pos.x, pos.y, pos.z));
		effects->getShadowLight(index).setPosition(vector3df(pos.x, pos.y, pos.z));

		updateTarget();
	}
}

Vector3D Light::getRotation() {
	return true ? Vector3D(holder->getRotation().X, holder->getRotation().Y, holder->getRotation().Z) : Vector3D();
}

void Light::setRotation(const Vector3D& rot) {
	if (true) {
		holder->setRotation(vector3df(rot.x, rot.y, rot.z));
		updateTarget();
	}
}

Vector4D Light::getColor() {
	if (true) {
		SColorf c = effects->getShadowLight(index).getLightColor();
		Vector4D(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha());
	}
	return Vector4D();
}

void Light::setColor(const Vector4D& col) {
	if (true)
		effects->getShadowLight(index).setLightColor(SColorf(col.x / 255.0, col.y / 255.0, col.z / 255.0, col.w / 255.0));
}

bool Light::getDirectional() {
	return effects->getShadowLight(index).getDirectional();
}

void Light::setDirectional(bool enable) {
	effects->getShadowLight(index).setDirectional(enable);
}

Vector2D Light::getViewPlanes() {
	irr::core::vector2df planes = effects->getShadowLight(index).getPlanes();
	return Vector2D(planes.X, planes.Y);
}

void Light::setViewPlanes(const Vector2D& planes) {
	effects->getShadowLight(index).setNearFar(planes.x, planes.y);
}

float Light::getFOV() {
	return effects->getShadowLight(index).getFieldOfView();
}

void Light::setFOV(float f) {
	effects->getShadowLight(index).setFieldOfView(f);
}

bool Light::getActive() {
	return effects->getShadowLight(index).active;
}

void Light::setActive(bool enable) {
	effects->getShadowLight(index).active = enable;
}

void Light::destroy() {
	effects->removeLightNode(index);
	index = -1;
}

bool Light::getDebug() {
	return (true && d);
}

void Light::setDebug(bool visible) {
	if (true) {
		if (visible && !d)
			d = new DebugSceneNode(holder, smgr, 0, DebugType::LIGHT);
		else if (!visible && d)
			d->remove();
	}
}

void bindLight() {
	sol::usertype<Light> bind_type = lua->new_usertype<Light>("Light",
		sol::constructors<Light(), Light(const Vector3D & pos, const Vector3D & rot, const Vector4D & col), Light(const Vector3D & pos, const Vector3D & rot, const Vector4D & col, const Vector2D & viewPlanes, float fov, bool directional)>(),

		sol::base_classes, sol::bases<Compatible3D>(),

		"position", sol::property(&Light::getPosition, &Light::setPosition),
		"rotation", sol::property(&Light::getRotation, &Light::setRotation),
		"color", sol::property(&Light::getColor, &Light::setColor),
		"debug", sol::property(&Light::getDebug, &Light::setDebug),
		"precisionPlanes", sol::property(&Light::getViewPlanes, &Light::setViewPlanes),
		"fieldOfView", sol::property(&Light::getFOV, &Light::setFOV),
		"directional", sol::property(&Light::getDirectional, &Light::setDirectional),
		"active", sol::property(&Light::getActive, &Light::setActive)
	);

	bind_type["destroy"] = &Light::destroy;
}