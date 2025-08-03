#include "LegacyLight.h"

LegacyLight::LegacyLight(int type, const Vector3D& pos, const Vector3D& rot, const Vector4D& color) {
	vector3df p = vector3df(pos.x, pos.y, pos.z);
	vector3df r = vector3df(rot.x, rot.y, rot.z);
	SColorf col = SColorf(color.x / 255.0, color.y / 255.0, color.z / 255.0, color.w / 255.0);
	light = smgr->addLightSceneNode(0, p, col);
	light->setLightType((E_LIGHT_TYPE)type);
	light->setRotation(r);
}

LegacyLight::LegacyLight() : LegacyLight(0, Vector3D(), Vector3D(), Vector4D(255)) {}

LegacyLight::LegacyLight(const LegacyLight& other) : LegacyLight(other.light->getLightType(), 
													 Vector3D(other.light->getPosition().X, other.light->getPosition().Y, other.light->getPosition().Z),
													 Vector3D(other.light->getRotation().X, other.light->getRotation().Y, other.light->getRotation().Z),
													 Vector4D(other.light->getLightData().DiffuseColor.getRed(), other.light->getLightData().DiffuseColor.getGreen(), other.light->getLightData().DiffuseColor.getBlue(), other.light->getLightData().DiffuseColor.getAlpha()))
{
	
}

LegacyLight::LegacyLight(int type) : LegacyLight(type, Vector3D(), Vector3D(), Vector4D()) {}

LegacyLight::LegacyLight(const Vector3D& pos) : LegacyLight(0, pos, Vector3D(), Vector4D()) {}

Vector3D LegacyLight::getPosition() {
	return true ? Vector3D(light->getPosition().X, light->getPosition().Y, light->getPosition().Z) : Vector3D();
}

void LegacyLight::setPosition(const Vector3D& pos) {
	if (light)
		light->setPosition(vector3df(pos.x, pos.y, pos.z));
}

Vector3D LegacyLight::getRotation() {
	return light ? Vector3D(light->getRotation().X, light->getRotation().Y, light->getRotation().Z) : Vector3D();
}

void LegacyLight::setRotation(const Vector3D& rot) {
	if (light)
		light->setRotation(vector3df(rot.x, rot.y, rot.z));
}

Vector4D LegacyLight::getLightColor() {
	if (light) {
		SColorf c = light->getLightData().DiffuseColor;
		Vector4D(c.getRed() * 255.0, c.getGreen() * 255.0, c.getBlue() * 255.0, c.getAlpha() * 255.0);
	}
	return Vector4D();
}

void LegacyLight::setLightColor(const Vector4D& col) {
	if (light)
		light->getLightData().DiffuseColor = (SColorf(col.x / 255.0, col.y / 255.0, col.z / 255.0, col.w / 255.0));
}

Vector4D LegacyLight::getAmbientColor() {
	if (light) {
		SColorf c = light->getLightData().AmbientColor;
		Vector4D(c.getRed() * 255.0, c.getGreen() * 255.0, c.getBlue() * 255.0, c.getAlpha() * 255.0);
	}
	return Vector4D();
}

void LegacyLight::setAmbientColor(const Vector4D& col) {
	if (light)
		light->getLightData().AmbientColor = (SColorf(col.x / 255.0, col.y / 255.0, col.z / 255.0, col.w / 255.0));
}

Vector4D LegacyLight::getSpecColor() {
	if (light) {
		SColorf c = light->getLightData().SpecularColor;
		Vector4D(c.getRed() * 255.0, c.getGreen() * 255.0, c.getBlue() * 255.0, c.getAlpha() * 255.0);
	}
	return Vector4D();
}

void LegacyLight::setSpecColor(const Vector4D& col) {
	if (light)
		light->getLightData().SpecularColor = (SColorf(col.x / 255.0, col.y / 255.0, col.z / 255.0, col.w / 255.0));
}

int LegacyLight::getType() {
	return light ? (int)light->getLightData().Type : 0;
}

void LegacyLight::setType(int i) {
	if (light) {
		light->getLightData().Type = (E_LIGHT_TYPE)i;
		if (d)
			d->val1 = light->getLightData().Type == E_LIGHT_TYPE::ELT_POINT ? 1 : 0;
	}
}

Vector2D LegacyLight::getCones() {
	return light ? Vector2D(light->getLightData().InnerCone, light->getLightData().OuterCone) : Vector2D();
}

void LegacyLight::setCones(const Vector2D& cones) {
	if (light) {
		light->getLightData().InnerCone = cones.x;
		light->getLightData().OuterCone = cones.y;
	}
}

float LegacyLight::getRadius() {
	return light ? light->getLightData().Radius : 0.0;
}

void LegacyLight::setRadius(float r) {
	if (light) {
		light->getLightData().Radius = r;
		if (d)
			d->rad = light->getLightData().Radius;
	}
}

Vector3D LegacyLight::getAttenuation() {
	return light ? Vector3D(light->getLightData().Attenuation.X, light->getLightData().Attenuation.Y, light->getLightData().Attenuation.Z) : Vector3D();
}

void LegacyLight::setAttenuation(const Vector3D& a) {
	if (light)
		light->getLightData().Attenuation = vector3df(a.x, a.y, a.z);
}

float LegacyLight::getFalloff() {
	return light ? light->getLightData().Falloff : 0.0;
}

void LegacyLight::setFalloff(float f) {
	if (light)
		light->getLightData().Falloff = f;
}

void LegacyLight::destroy() {
	if (light)
		light->remove();
}

bool LegacyLight::getDebug() {
	return (light && d);
}

void LegacyLight::setDebug(bool visible) {
	if (light) {
		if (visible && !d) {
			d = new DebugSceneNode(light, smgr, 0, DebugType::LIGHT);
			d->rad = light->getLightData().Radius;
			d->val1 = light->getLightData().Type == E_LIGHT_TYPE::ELT_POINT ? 1 : 0;
		}
		else if (!visible && d)
			d->remove();
	}
}

bool LegacyLight::getVisible() {
	return light ? light->isVisible() : false;
}

void LegacyLight::setVisible(bool visible) {
	if (light)
		light->setVisible(visible);
}

void bindLegacyLight() {
	sol::usertype<LegacyLight> bind_type = lua->new_usertype<LegacyLight>("LegacyLight",
		sol::constructors<LegacyLight(), LegacyLight(const LegacyLight& other), LegacyLight(int type), LegacyLight(const Vector3D& pos), LegacyLight(int type, const Vector3D & pos, const Vector3D & rot, const Vector4D & color)>(),

		sol::base_classes, sol::bases<Compatible3D>(),

		"position", sol::property(&LegacyLight::getPosition, &LegacyLight::setPosition),
		"rotation", sol::property(&LegacyLight::getRotation, &LegacyLight::setRotation),
		"type", sol::property(&LegacyLight::getType, &LegacyLight::setType),
		"diffuseColor", sol::property(&LegacyLight::getLightColor, &LegacyLight::setLightColor),
		"ambientColor", sol::property(&LegacyLight::getAmbientColor, &LegacyLight::setAmbientColor),
		"debug", sol::property(&LegacyLight::getDebug, &LegacyLight::setDebug),
		"cones", sol::property(&LegacyLight::getCones, &LegacyLight::setCones),
		"active", sol::property(&LegacyLight::getVisible, &LegacyLight::setVisible),
		"radius", sol::property(&LegacyLight::getRadius, &LegacyLight::setRadius),
		"attenuation", sol::property(&LegacyLight::getAttenuation, &LegacyLight::setAttenuation),
		"falloff", sol::property(&LegacyLight::getFalloff, &LegacyLight::setFalloff),
		"specularColor", sol::property(&LegacyLight::getSpecColor, &LegacyLight::setSpecColor)
	);

	bind_type["destroy"] = &LegacyLight::destroy;
}