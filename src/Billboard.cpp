#include "Billboard.h"

Billboard::Billboard() {
    bb = smgr->addBillboardSceneNode();
    bb->grab();
}

Billboard::Billboard(const Billboard& other) {
    bb = smgr->addBillboardSceneNode();
    bb->grab();
    bb = other.bb;
    myMaterial = other.myMaterial;
    bb->getMaterial(0) = other.myMaterial;
}

Billboard::Billboard(const Material& material) {
    bb = smgr->addBillboardSceneNode();
    bb->grab();
    loadMaterial(material);
}

Vector2D Billboard::getSize() {
    return Vector2D(bb->getSize().Width, bb->getSize().Height);
}

void Billboard::setSize(const Vector2D& size) {
    bb->setSize(irr::core::vector2df(size.x, size.y));
}

bool Billboard::loadMaterial(const Material& material) {
    myMaterial = material.mat;
    bb->getMaterial(0) = myMaterial;
    return true;
}

Vector3D Billboard::getRotLock() {
    return Vector3D(bb->getRotLock().X, bb->getRotLock().Y, bb->getRotLock().Z);
}

void Billboard::setRotLock(const Vector3D& rot) {
    bb->setRotLock(irr::core::vector3d<bool>(rot.x, rot.y, rot.z));
}

irr::f32 Billboard::getYPivot() {
    return bb->getPivotY();
}

void Billboard::setYPivot(float y) {
    bb->setPivotY(y);
}

void Billboard::destroy() {
    if (bb) {
        bb->remove();
    }
}

#include "Proxy.h"
void bindBillboard() {
    sol::usertype<Billboard> bindType = lua->new_usertype<Billboard>("Billboard",
        sol::constructors<Billboard(), Billboard(const Material & material)>(),

        sol::base_classes, sol::bases<Compatible3D>(),

        "size", sol::property(
            [](Billboard& c) { return Vector2DProxy{ [&] { return c.getSize(); }, [&](auto v) { c.setSize(v); } }; },
            [](Billboard& c, const Vector2D& v) { c.setSize(v); }
        ),

        "yPivot", sol::property(&Billboard::getYPivot, &Billboard::setYPivot),
        "lockAxis", sol::property(&Billboard::getRotLock, &Billboard::setRotLock)
    );

    bindType["loadMaterial"] = &Billboard::loadMaterial;
    bindType["destroy"] = &Billboard::destroy;
    bindType["setParent"] = &Billboard::setParent;
}