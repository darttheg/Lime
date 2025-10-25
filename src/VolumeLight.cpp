#include "VolumeLight.h"

VolumeLight::VolumeLight() : VolumeLight(Vector4D(255, 0, 0, 255), Vector4D(0, 255, 0, 255)) {}

VolumeLight::VolumeLight(const Vector4D& footCol, const Vector4D& tailCol) : VolumeLight(footCol, tailCol, 5, 5) {}

VolumeLight::VolumeLight(const Vector4D& footCol, const Vector4D& tailCol, int segmentU, int segmentV) {
	SColor footColIrr = SColor(footCol.getW(), footCol.getX(), footCol.getY(), footCol.getZ());
	SColor tailColIrr = SColor(tailCol.getW(), tailCol.getX(), tailCol.getY(), tailCol.getZ());
	vol = smgr->addVolumeLightSceneNode(nullptr, -1, segmentU, segmentV, footColIrr, tailColIrr);
}

void VolumeLight::destroy() {
	if (vol) smgr->addToDeletionQueue(vol);
}

int VolumeLight::getSegmentU() {
	return vol->getSubDivideU();
}

void VolumeLight::setSegmentU(int u) {
	if (vol) vol->setSubDivideU(u);
}

int VolumeLight::getSegmentV() {
	return vol->getSubDivideV();
}

void VolumeLight::setSegmentV(int v) {
	if (vol) vol->setSubDivideU(v);
}

void VolumeLight::setSegments(int uv) {
	if (vol) {
		vol->setSubDivideU(uv);
		vol->setSubDivideV(uv);
	}
}

Vector4D VolumeLight::getFootColor() {
	Vector4D out = Vector4D(vol->getFootColor().getRed(), vol->getFootColor().getGreen(), vol->getFootColor().getBlue(), 255 + vol->getFootColor().getAlpha());
	return out;
}

void VolumeLight::setFootColor(const Vector4D& c) {
	if (vol) vol->setFootColor(SColor(255 - c.getW(), c.getX(), c.getY(), c.getZ()));
}

Vector4D VolumeLight::getTailColor() {
	Vector4D out = Vector4D(vol->getTailColor().getRed(), vol->getTailColor().getGreen(), vol->getTailColor().getBlue(), 255 + vol->getTailColor().getAlpha());
	return out;
}

void VolumeLight::setTailColor(const Vector4D& c) {
	if (vol) vol->setTailColor(SColor(255 - c.getW(), c.getX(), c.getY(), c.getZ()));
}

void bindVolumeLight() {
	sol::usertype<VolumeLight> bindType = lua->new_usertype<VolumeLight>("VolumeLight",
		sol::constructors<VolumeLight(), VolumeLight(const Vector4D & footCol, const Vector4D & tailCol), VolumeLight(const Vector4D & footCol, const Vector4D & tailCol, int segmentU, int segmentV)>(),

		sol::base_classes, sol::bases<Compatible3D>(),
		sol::meta_function::type, [](const VolumeLight&) { return "VolumeLight"; },

		"headColor", sol::property(
			[](VolumeLight& c) { return Vector4D{ [&] { return c.getFootColor(); }, [&](auto v) { c.setFootColor(v); } }; },
			[](VolumeLight& c, const Vector4D& v) { c.setFootColor(v); }
		),
		"tailColor", sol::property(
			[](VolumeLight& c) { return Vector4D{ [&] { return c.getTailColor(); }, [&](auto v) { c.setTailColor(v); } }; },
			[](VolumeLight& c, const Vector4D& v) { c.setTailColor(v); }
		),

		"resolutionU", sol::property(&VolumeLight::getSegmentU, &VolumeLight::setSegmentU),
		"resolutionV", sol::property(&VolumeLight::getSegmentV, &VolumeLight::setSegmentV)
	);

	bindType["setResolution"] = &VolumeLight::setSegments;
}