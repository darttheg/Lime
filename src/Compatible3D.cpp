#include "Compatible3D.h"

void Compatible3D::setParent(sol::optional<Compatible3D*> parent) {
    irr::scene::ISceneNode* node = getNode();
    if (!node) return;

    node->setParent(*parent ? (*parent)->getNode() : nullptr);
}

Vector3D Compatible3D::getAbsPos() {
    irr::scene::ISceneNode* node = getNode();
    if (!node) return Vector3D();

    irr::core::vector3df pos = node->getAbsolutePosition();
    return Vector3D(pos.X, pos.Y, pos.Z);
}

Vector3D Compatible3D::getAbsRot() {
    irr::scene::ISceneNode* node = getNode();
    if (!node) return Vector3D();

    irr::core::vector3df rot = node->getAbsoluteTransformation().getRotationDegrees();
    return Vector3D(rot.X, rot.Y, rot.Z);
}

Vector3D Compatible3D::getAbsScale() {
    irr::scene::ISceneNode* node = getNode();
    if (!node) return Vector3D();

    irr::core::vector3df scale = node->getAbsoluteTransformation().getScale();
    return Vector3D(scale.X, scale.Y, scale.Z);
}

void Compatible3D::updateAbsPos() {
    if (!getNode()) return;

    getNode()->updateAbsolutePosition();
}

sol::table Compatible3D::getEntry() {
    // Creates on get, find solution later?

    if (hasEntry)
        return irrHandler->comp3dmap[getNode()];

    createEntry();
    return irrHandler->comp3dmap[getNode()];
}

void Compatible3D::setEntry(sol::table obj) {
    createEntry();

    if (obj.is<sol::nil_t>())
        irrHandler->comp3dmap.erase(getNode());
    else
        irrHandler->comp3dmap[getNode()] = obj;
}

void Compatible3D::createEntry() {
    if (getNode() && !hasEntry)
    {
        irrHandler->comp3dmap[getNode()] = lua->create_table_with();
        hasEntry = true;
    }
}

void Compatible3D::destroyEntry() {
    auto it = irrHandler->comp3dmap.find(getNode());
    if (it != irrHandler->comp3dmap.end())
        irrHandler->comp3dmap.erase(it);
}

// New general 3D functions

Vector3D Compatible3D::getPosition() {
    if (!getNode()) return Vector3D();
    return Vector3D(getNode()->getPosition().X, getNode()->getPosition().Y, getNode()->getPosition().Z);
}

void Compatible3D::setPosition(const Vector3D& pos) {
    if (getNode())
        getNode()->setPosition(irr::core::vector3df(pos.x, pos.y, pos.z));
}

Vector3D Compatible3D::getRotation() {
    if (!getNode()) return Vector3D();
    return Vector3D(getNode()->getRotation().X, getNode()->getRotation().Y, getNode()->getRotation().Z);
}

void Compatible3D::setRotation(const Vector3D& rot) {
    // float clampedX = std::clamp(rot.x, -89.0f, 89.0f); From Camera
    // getNode()->setRotation(irr::core::vector3df(clampedX, rot.y, rot.z));
    if (getNode())
        getNode()->setRotation(irr::core::vector3df(rot.x, rot.y, rot.z));
}

Vector3D Compatible3D::getScale() {
    if (!getNode()) return Vector3D();
    return Vector3D(getNode()->getScale().X, getNode()->getScale().Y, getNode()->getScale().Z);
}

void Compatible3D::setScale(const Vector3D& scale) {
    if (getNode())
        getNode()->setScale(irr::core::vector3df(scale.x, scale.y, scale.z));
}

bool Compatible3D::getVisibility() const {
    return getNode() ? getNode()->isVisible() : false;
}

void Compatible3D::setVisibility(bool visible) {
    if (getNode()) getNode()->setVisible(visible);
}

int Compatible3D::getID() {
    return getNode() ? getNode()->getID() : -1;
}

void Compatible3D::setID(int i) {
    if (getNode()) {
        getNode()->setID(i);
    }
}

void bindCompatible3D() {
    sol::usertype<Compatible3D> bindType = lua->new_usertype<Compatible3D>("Compatible3D",
        "attributes", sol::property(&Compatible3D::getEntry, &Compatible3D::setEntry),

        "position", sol::property(
            [](Compatible3D& c) { return Vector3D{ [&] { return c.getPosition(); }, [&](auto v) { c.setPosition(v); } }; },
            [](Compatible3D& c, const Vector3D& v) { c.setPosition(v); }
        ),
        "rotation", sol::property(
            [](Compatible3D& c) { return Vector3D{ [&] { return c.getRotation(); }, [&](auto v) { c.setRotation(v); } }; },
            [](Compatible3D& c, const Vector3D& v) { c.setRotation(v); }
        ),
        "scale", sol::property(
            [](Compatible3D& c) { return Vector3D{ [&] { return c.getScale(); }, [&](auto v) { c.setScale(v); } }; },
            [](Compatible3D& c, const Vector3D& v) { c.setScale(v); }
        ),

        "visible", sol::property(&Compatible3D::getVisibility, &Compatible3D::setVisibility),
        "ID", sol::property(&Compatible3D::getID, &Compatible3D::setID)
    );

    bindType["setParent"] = &Compatible3D::setParent;
    bindType["getAbsolutePosition"] = &Compatible3D::getAbsPos;
    bindType["getAbsoluteRotation"] = &Compatible3D::getAbsRot;
    bindType["getAbsoluteScale"] = &Compatible3D::getAbsScale;
    bindType["updateAbsolutePosition"] = &Compatible3D::updateAbsPos;
}