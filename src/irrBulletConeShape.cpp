// Copyright (C) 2014- Danyal Zia
// Copyright (C) 2009-2013 Josiah Hartzell (Skyreign Software)
// This file is part of the "irrBullet" Bullet physics wrapper.
// For conditions of distribution and use, see copyright notice in irrBullet.h
// The above copyright notice and its accompanying information must remain here.

#include <ISceneNode.h>
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "irrBulletConeShape.h"

using namespace irr;
using namespace core;
using namespace scene;

IConeShape::IConeShape(ISceneNode* const n, f32 m, bool overrideMargin)
{
	node = n;
	mass = m;

	type = ECollisionShapeType::ECST_CONE;

	createShape(overrideMargin);
}

void IConeShape::createShape(bool overrideMargin)
{
	node->updateAbsolutePosition();
	const aabbox3df& box = node->getTransformedBoundingBox();
	const vector3df& diag = (box.MaxEdge - box.getCenter()) + f32((overrideMargin) ? 0.04 : 0.0);
	const f32 radius = f32(diag.getLength() * 0.5f);

	const f32 scale = node->getParent()->getScale().Y;
	const f32 height = box.getExtent().Y * 0.5f * scale;

	shape = new btConeShape(radius, height);
	calculateLocalInertia(getMass(), vector3df(0.0f, 0.0f, 0.0f));
}

IConeShape::~IConeShape()
{
}
