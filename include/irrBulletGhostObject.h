// Copyright (C) 2014- Danyal Zia
// Copyright (C) 2009-2013 Josiah Hartzell (Skyreign Software)
// This file is part of the "irrBullet" Bullet physics wrapper.
// For conditions of distribution and use, see copyright notice in irrBullet.h
// The above copyright notice and its accompanying information must remain here.

#pragma once

#include "irrBulletCommon.h"
#include "irrBulletCompileConfig.h"
#include "irrBulletCollisionObject.h"
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "irrBulletRayCastVehicle.h"
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <vector>

class IMotionState;


/// The rigid body is the main type for all "hard" simulation objects (The opposite of a soft body).
class IGhostObject : public ICollisionObject
{
public:
    /// @param collShape The collision shape for this body to use.
    IGhostObject(irrBulletWorld* const world, ICollisionShape* const collShape);

	IGhostObject(const IGhostObject& other) = default;

	IGhostObject& operator=(const IGhostObject& other) = default;

	// Move constructor
	IGhostObject(const IGhostObject&& other)
	{
		*this = std::move(other);
	}

	// Move assignment operator
	IGhostObject& operator=(IGhostObject&& other)
	{
		if (this != &other)
		{
			delete shape;

			shape = other.shape;
			worldTransform = other.worldTransform;
			motionState = other.motionState;

			shape = nullptr;
			motionState = nullptr;
		}
		return *this;
	}

	virtual ~IGhostObject();

    void setTranslation(const irr::core::vector3df& v);

    /// Aligns the rigidbody to point to the given target position. Useful for things like guided weapons and artificial intelligence.
    void faceTarget(const irr::core::vector3df& targetPosition);

    /// Sets the collision shape for the object to use.
    void setCollisionShape(ICollisionShape* const shape);

    bool compare(btGhostObject* const other) const
    {
        irr::u32 otherID = static_cast<SCollisionObjectIdentification*>(other->getUserPointer())->getCollisionObject()->getUniqueID();
        return (this->uniqueID == otherID);
    };

    bool compare(const btGhostObject& other) const
    {
        irr::u32 otherID = static_cast<SCollisionObjectIdentification*>(other.getUserPointer())->getCollisionObject()->getUniqueID();
        return (this->uniqueID == otherID);
    };



    /// @return A pointer to the underlying btRigidBody.
    btGhostObject* getPointer() const { return static_cast<btGhostObject*>(object); };

    bool checkCollideWithOverride(ICollisionObject* const co);


    /// @return Current collision shape in use by the object.
    ICollisionShape* getCollisionShape() const { return shape; }


protected:
    ICollisionShape *shape;
    irr::core::matrix4 worldTransform;
    IMotionState *motionState;
};
