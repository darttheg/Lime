#pragma once

#include <irrlicht.h>
#include <sol/sol.hpp>
#include "LuaLime.h"
#include "IrrManagers.h"

class Compatible3D {
public:
    virtual ~Compatible3D() = default;

    virtual irr::scene::ISceneNode* getNode() const = 0;

    void setParent(sol::optional<Compatible3D*> parent) {
        irr::scene::ISceneNode* node = getNode();
        if (!node) return;

        node->setParent(*parent ? (*parent)->getNode() : nullptr);
    }

    Vector3D getAbsPos() {
        irr::scene::ISceneNode* node = getNode();
        if (!node) return Vector3D();

        irr::core::vector3df pos = node->getAbsolutePosition();
        return Vector3D(pos.X, pos.Y, pos.Z);
    }

    Vector3D getAbsRot() {
        irr::scene::ISceneNode* node = getNode();
        if (!node) return Vector3D();

        irr::core::vector3df rot = node->getAbsoluteTransformation().getRotationDegrees();
        return Vector3D(rot.X, rot.Y, rot.Z);
    }

    Vector3D getAbsScale() {
        irr::scene::ISceneNode* node = getNode();
        if (!node) return Vector3D();

        irr::core::vector3df scale = node->getAbsoluteTransformation().getScale();
        return Vector3D(scale.X, scale.Y, scale.Z);
    }

    void updateAbsPos() {
        if (!getNode()) return;

        getNode()->updateAbsolutePosition();
    }

    sol::object getEntry() {
        auto it = irrHandler->comp3dmap.find(getNode());
        if (it != irrHandler->comp3dmap.end())
            return it->second;
        return sol::nil;
    }

	void setEntry(sol::object obj) {
		if (obj.is<sol::nil_t>())
			irrHandler->comp3dmap.erase(getNode());
		else
			irrHandler->comp3dmap[getNode()] = obj;
	}

    void destroyEntry() {
        auto it = irrHandler->comp3dmap.find(getNode());
        if (it != irrHandler->comp3dmap.end())
            irrHandler->comp3dmap.erase(it);
    }

    void createEntry() {
        if (irrHandler->comp3dmap.find(getNode()) == irrHandler->comp3dmap.end())
            irrHandler->comp3dmap[getNode()] = lua->create_table_with();
    }
};

inline void bindCompatible3D() {
    sol::usertype<Compatible3D> bind_type = lua->new_usertype<Compatible3D>("Compatible3D",
        "attributes", sol::property(&Compatible3D::getEntry, &Compatible3D::setEntry)
    );

    bind_type["setParent"] = &Compatible3D::setParent;
    bind_type["getAbsolutePosition"] = &Compatible3D::getAbsPos;
    bind_type["getAbsoluteRotation"] = &Compatible3D::getAbsRot;
    bind_type["getAbsoluteScale"] = &Compatible3D::getAbsScale;
    bind_type["updateAbsolutePosition"] = &Compatible3D::updateAbsPos;
}