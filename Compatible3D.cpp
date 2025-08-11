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

void bindCompatible3D() {
    sol::usertype<Compatible3D> bind_type = lua->new_usertype<Compatible3D>("Compatible3D",
        "attributes", sol::property(&Compatible3D::getEntry, &Compatible3D::setEntry)
    );

    bind_type["setParent"] = &Compatible3D::setParent;
    bind_type["getAbsolutePosition"] = &Compatible3D::getAbsPos;
    bind_type["getAbsoluteRotation"] = &Compatible3D::getAbsRot;
    bind_type["getAbsoluteScale"] = &Compatible3D::getAbsScale;
    bind_type["updateAbsolutePosition"] = &Compatible3D::updateAbsPos;
}