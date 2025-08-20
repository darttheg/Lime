#include "WaterMesh.h"

Water::Water(float h, float s, float l, const Vector2D& ts, const Vector2D& tc, const Vector2D& tr, const Material& m)
    : height(h), speed(s), length(l), tileSize(ts.x, ts.y), tileCount(tc.x, tc.y), texRepeat(tr.x, tr.y), material(m.mat), water(nullptr), rawMesh(nullptr) {
    createRaw();
    refreshMesh();
}

Water::Water() : Water(2, 300, 30, Vector2D(20, 20), Vector2D(40, 40), Vector2D(30, 30), Material()) {}

Water::Water(const Material& m) : Water(2, 300, 30, Vector2D(20, 20), Vector2D(40, 40), Vector2D(10, 10), m) {}

Water::Water(const Vector2D& ts, const Vector2D& tc, const Vector2D& tr) : Water(2, 300, 30, ts, tc, tr, Material()) {}

Water::Water(sol::table tbl) : Water() {
    if (tbl["height"]) height = tbl["height"];
    if (tbl["speed"]) speed = tbl["speed"];
    if (tbl["length"]) length = tbl["length"];

    if (tbl["tileSize"]) {
        tileSize.X = TblFrom2D(tbl["tileSize"]).x;
        tileSize.Y = TblFrom2D(tbl["tileSize"]).y;
    }

    if (tbl["tileCount"]) {
        tileCount.X = TblFrom2D(tbl["tileCount"]).x;
        tileCount.Y = TblFrom2D(tbl["tileCount"]).y;
    }

    if (tbl["textureRepeat"]) {
        texRepeat.X = TblFrom2D(tbl["textureRepeat"]).x;
        texRepeat.Y = TblFrom2D(tbl["textureRepeat"]).y;
    }

    if (tbl["material"]) material = tbl["material"];
}

void Water::refreshMesh() {
    irr::core::vector3df pos = water ? water->getPosition() : irr::core::vector3df();
    irr::core::vector3df rot = water ? water->getRotation() : irr::core::vector3df();
    irr::core::vector3df scale = water ? water->getScale() : irr::core::vector3df(1, 1, 1);

    if (water) water->remove(); // Add createEntry();, copy over contents from pointer

    water = smgr->addWaterSurfaceSceneNode(rawMesh, height, speed, length, 0, 0, pos, rot, scale);
    water->getMaterial(0) = material;
}

void Water::createRaw() {
    rawMesh = smgr->addHillPlaneMesh("myHill",
        core::dimension2d<f32>(tileSize.X, tileSize.Y),
        core::dimension2d<u32>(tileCount.X, tileCount.Y),
        &material,
        0, core::dimension2d<f32>(0, 0),
        core::dimension2d<f32>(texRepeat.X, texRepeat.Y))->getMesh(0);

    refreshMesh();
}

void Water::destroy() {
    if (water) {
        destroyEntry();
        water->remove();
    }
}

float Water::getHeight() {
    return height;
}

void Water::setHeight(float i) {
    height = i;
    refreshMesh();
}

float Water::getSpeed() {
    return speed;
}

void Water::setSpeed(float i) {
    speed = i;
    refreshMesh();
}

float Water::getLength() {
    return length;
}

void Water::setLength(float i) {
    length = i;
    refreshMesh();
}

bool Water::getVisibility() const {
    return water ? water->isVisible() : false;
}

void Water::setVisibility(bool visible) {
    if (water) water->setVisible(visible);
}

Vector3D Water::getPosition() {
    return water ? Vector3D(water->getPosition().X, water->getPosition().Y, water->getPosition().Z) : Vector3D();
}

void Water::setPosition(const Vector3D& pos) {
    if (water) water->setPosition(irr::core::vector3df(pos.x, pos.y, pos.z));
}

Vector3D Water::getRotation() {
    return water ? Vector3D(water->getRotation().X, water->getRotation().Y, water->getRotation().Z) : Vector3D();
}

void Water::setRotation(const Vector3D& rot) {
    if (water) water->setRotation(irr::core::vector3df(rot.x, rot.y, rot.z));
}

Vector3D Water::getScale() {
    return water ? Vector3D(water->getScale().X, water->getScale().Y, water->getScale().Z) : Vector3D();
}

void Water::setScale(const Vector3D& scale) {
    if (water) water->setScale(irr::core::vector3df(scale.x, scale.y, scale.z));
}

void Water::loadMaterial(const Material& m) {
    material = m.mat;
    water->getMaterial(0) = material;
}

Vector2D Water::getTileSize() {
    return Vector2D(tileSize.X, tileSize.Y);
}

void Water::setTileSize(const Vector2D& other) {
    tileSize = irr::core::vector2df(other.x, other.y);
}

Vector2D Water::getTileCount() {
    return Vector2D(tileCount.X, tileCount.Y);
}

void Water::setTileCount(const Vector2D& other) {
    tileCount = irr::core::vector2df(other.x, other.y);
}

Vector2D Water::getTexRepeat() {
    return Vector2D(texRepeat.X, texRepeat.Y);
}

void Water::setTexRepeat(const Vector2D& other) {
    texRepeat = irr::core::vector2df(other.x, other.y);
}

void bindWater() {
    sol::usertype<Water> bind_type = lua->new_usertype<Water>("Water",
        sol::constructors<
        Water(),
        Water(const Vector2D & ts, const Vector2D & tc, const Vector2D & tr),
        Water(const Material & m),
        Water(sol::table tbl)>(),

        sol::base_classes, sol::bases<Compatible3D>(),

        "position", sol::property(&Water::getPosition, &Water::setPosition),
        "rotation", sol::property(&Water::getRotation, &Water::setRotation),
        "scale", sol::property(&Water::getScale, &Water::setScale),
        "visible", sol::property(&Water::getVisibility, &Water::setVisibility),
        "height", sol::property(&Water::getHeight, &Water::setHeight),
        "speed", sol::property(&Water::getSpeed, &Water::setSpeed),
        "length", sol::property(&Water::getLength, &Water::setLength),
        "tileSize", sol::property(&Water::getTileSize, &Water::setTileSize),
        "tileCount", sol::property(&Water::getTileCount, &Water::setTileCount),
        "texRepeat", sol::property(&Water::getTexRepeat, &Water::setTexRepeat));

    bind_type["destroy"] = &Water::destroy;
    bind_type["loadMaterial"] = &Water::loadMaterial;
    bind_type["setParent"] = &Water::setParent;
}