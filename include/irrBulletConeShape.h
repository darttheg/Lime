// Copyright (C) 2014- Danyal Zia
// Copyright (C) 2009-2013 Josiah Hartzell (Skyreign Software)
// This file is part of the "irrBullet" Bullet physics wrapper.
// For conditions of distribution and use, see copyright notice in irrBullet.h
// The above copyright notice and its accompanying information must remain here.

#pragma once

#include "irrBulletCollisionShape.h"

class IConeShape : public ICollisionShape
{
public:
	IConeShape(irr::scene::ISceneNode* const n, irr::f32 m, bool overrideMargin = false);

	IConeShape(const IConeShape& other) = default;

	IConeShape& operator=(const IConeShape& other) = default;

	// move constructor
	IConeShape(const IConeShape&& other)
	{
		*this = std::move(other);
	}

	// Move assignment operator
	IConeShape& operator=(IConeShape&& other)
	{
		if (this != &other)
		{
			delete node;

			node = other.node;
			mass = other.mass;

			other.node = nullptr;
			other.mass = 0;
		}
		return *this;
	}

	virtual ~IConeShape();

protected:
	void createShape(bool overrideMargin);
};
