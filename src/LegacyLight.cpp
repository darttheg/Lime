#include "LegacyLight.h"

LegacyLight::LegacyLight(const Vector3D& pos, const Vector3D& rot, int type, const Vector4D& color) {
	vector3df p = vector3df(pos.x, pos.y, pos.z);
	vector3df r = vector3df(rot.x, rot.y, rot.z);
	SColorf col = SColorf(color.x / 255.0, color.y / 255.0, color.z / 255.0, color.w / 255.0);
	light = smgr->addLightSceneNode(0, p, col);
	light->setLightType((E_LIGHT_TYPE)type);
	light->setRotation(r);
}

LegacyLight::LegacyLight() : LegacyLight(Vector3D(), Vector3D(), 0, Vector4D(255)) {}

LegacyLight::LegacyLight(const LegacyLight& other) : LegacyLight( 
													 Vector3D(other.light->getPosition().X, other.light->getPosition().Y, other.light->getPosition().Z),
													 Vector3D(other.light->getRotation().X, other.light->getRotation().Y, other.light->getRotation().Z),
													 other.light->getLightType(),
													 Vector4D(other.light->getLightData().DiffuseColor.getRed(), other.light->getLightData().DiffuseColor.getGreen(), other.light->getLightData().DiffuseColor.getBlue(), other.light->getLightData().DiffuseColor.getAlpha()))
{
	
}

LegacyLight::LegacyLight(int type) : LegacyLight(Vector3D(), Vector3D(), type, Vector4D()) {}

LegacyLight::LegacyLight(const Vector3D& pos) : LegacyLight(pos, Vector3D(), 0, Vector4D()) {}

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
	if (light) {
		destroyEntry();
		smgr->addToDeletionQueue(light);
	}
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


void bindLegacyLight() {
	sol::usertype<LegacyLight> bindType = lua->new_usertype<LegacyLight>("Light",
		sol::constructors<LegacyLight(), LegacyLight(int type), LegacyLight(const Vector3D& pos), LegacyLight(const Vector3D & pos, const Vector3D & rot, int type, const Vector4D & color)>(),

		sol::base_classes, sol::bases<Compatible3D>(),

		"type", sol::property(&LegacyLight::getType, &LegacyLight::setType),

		"diffuseColor", sol::property(
			[](LegacyLight& c) { return Vector4D{ [&] { return c.getLightColor(); }, [&](auto v) { c.setLightColor(v); } }; },
			[](LegacyLight& c, const Vector4D& v) { c.setLightColor(v); }
		),
		"ambientColor", sol::property(
			[](LegacyLight& c) { return Vector4D{ [&] { return c.getAmbientColor(); }, [&](auto v) { c.setAmbientColor(v); } }; },
			[](LegacyLight& c, const Vector4D& v) { c.setAmbientColor(v); }
		),
		"specularColor", sol::property(
			[](LegacyLight& c) { return Vector4D{ [&] { return c.getSpecColor(); }, [&](auto v) { c.setSpecColor(v); } }; },
			[](LegacyLight& c, const Vector4D& v) { c.setSpecColor(v); }
		),
		"cones", sol::property(
			[](LegacyLight& c) { return Vector2D{ [&] { return c.getCones(); }, [&](auto v) { c.setCones(v); } }; },
			[](LegacyLight& c, const Vector2D& v) { c.setCones(v); }
		),
		"attenuation", sol::property(
			[](LegacyLight& c) { return Vector3D{ [&] { return c.getAttenuation(); }, [&](auto v) { c.setAttenuation(v); } }; },
			[](LegacyLight& c, const Vector3D& v) { c.setAttenuation(v); }
		),

		"debug", sol::property(&LegacyLight::getDebug, &LegacyLight::setDebug),
		"radius", sol::property(&LegacyLight::getRadius, &LegacyLight::setRadius),
		"falloff", sol::property(&LegacyLight::getFalloff, &LegacyLight::setFalloff)
	);
}