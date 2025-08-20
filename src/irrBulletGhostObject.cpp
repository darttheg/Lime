// Copyright (C) 2014- Danyal Zia
// Copyright (C) 2009-2013 Josiah Hartzell (Skyreign Software)
// This file is part of the "irrBullet" Bullet physics wrapper.
// For conditions of distribution and use, see copyright notice in irrBullet.h
// The above copyright notice and its accompanying information must remain here.

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <ISceneManager.h>
#include <IFileSystem.h>
#include <IrrlichtDevice.h>
#include "irrBulletGhostObject.h"
#include "irrBulletCollisionShape.h"
#include "irrBulletMotionState.h"
#include "irrBulletCompileConfig.h"
#include "irrBulletWorld.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

IGhostObject::IGhostObject(irrBulletWorld* const world, ICollisionShape* const collShape)
{
    #ifdef IRRBULLET_DEBUG_MODE
        printf("irrBullet: Creating ghost object (%i)\n", this->getUniqueID());
    #endif
    shape = collShape;

	objectType = ECollisionObjectType::ECOT_GHOST_OBJECT;

    dynamicsWorld = world;

    worldTransform.setTranslation(shape->getSceneNode()->getPosition());
    worldTransform.setRotationDegrees(shape->getSceneNode()->getRotation());


    // Set the initial position of the object
    btTransformFromIrrlichtMatrix(worldTransform, internalTransform);
	//transform.setIdentity();
	//transform.setOrigin(irrlichtToBulletVector(pos));

    object = new btGhostObject();
    object->setCollisionShape(collShape->getPointer());

    //btGhostObject* ghost = (btGhostObject*) object;


    attributes = getCollisionShape()->getSceneNode()->getSceneManager()->getFileSystem()->createEmptyAttributes(
        getCollisionShape()->getSceneNode()->getSceneManager()->getVideoDriver());

    collID = new SCollisionObjectIdentification();
    collID->setCollisionObject(this);
    collID->setName("GhostObject");

    object->setUserPointer(collID);
}

void IGhostObject::setCollisionShape(ICollisionShape* const Shape)
{
	if (objectType == ECollisionObjectType::ECOT_GHOST_OBJECT)
    {
        shape = Shape;
        //shape->calculateLocalInertia(shape->getMass(), vector3df(0.0f,0.0f,0.0f));
        object->setCollisionShape(shape->getPointer());
    }
}

void IGhostObject::setTranslation(const vector3df &v)
{
    irr::core::matrix4 mat = getWorldTransform();
    mat.setTranslation(v);
    setWorldTransform(mat);
}


void IGhostObject::faceTarget( const vector3df& targetPosition )
{
    matrix4 mat = getWorldTransform();
    vector3df rot, dif = mat.getTranslation() - targetPosition;
    rot.Y = atan2( dif.X, dif.Z ) * 180.f / irr::core::PI;
    rot.X = atan2( dif.Y, sqrt( dif.X * dif.X + dif.Z * dif.Z ) ) * 180.f / irr::core::PI;
    mat.setRotationDegrees(rot);
    setWorldTransform(mat);
}



bool IGhostObject::checkCollideWithOverride(ICollisionObject* const co)
{
    return getPointer()->checkCollideWithOverride(co->getPointer());
}




IGhostObject::~IGhostObject()
{
    //if(IncludeNodeOnRemoval && shape)
        //shape->removeNode();

    if(shape)
        delete shape;
}


