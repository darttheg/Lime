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

Vector3D Billboard::getPosition() {
    return Vector3D(bb->getPosition().X, bb->getPosition().Y, bb->getPosition().Z);
}

void Billboard::setPosition(const Vector3D& pos) {
    bb->setPosition(irr::core::vector3df(pos.x, pos.y, pos.z));
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

bool Billboard::getVisible() {
    return bb ? bb->isVisible() : false;
}

void Billboard::setVisible(bool visible) {
    if (bb) {
        bb->setVisible(visible);
    }
}

void Billboard::destroy() {
    if (bb) {
        bb->remove();
    }
}

void bindBillboard() {
    sol::usertype<Billboard> bindType = lua->new_usertype<Billboard>("Billboard",
        sol::constructors<Billboard(), Billboard(const Billboard & other), Billboard(const Material & material)>(),

        sol::base_classes, sol::bases<Compatible3D>(),

        "position", sol::property(&Billboard::getPosition, &Billboard::setPosition),
        "size", sol::property(&Billboard::getSize, &Billboard::setSize),
        "yPivot", sol::property(&Billboard::getYPivot, &Billboard::setYPivot),
        "lockAxis", sol::property(&Billboard::getRotLock, &Billboard::setRotLock),
        "visible", sol::property(&Billboard::getVisible, &Billboard::setVisible)
    );

    bindType["loadMaterial"] = &Billboard::loadMaterial;
    bindType["destroy"] = &Billboard::destroy;
    bindType["setParent"] = &Billboard::setParent;
}