#pragma once

#include "Event.h"
#include "irrBullet.h"
#include <sol/sol.hpp>

// Interface for RigidBody3D and SoftBody3D
class PhysicsObject {
public:
	virtual ~PhysicsObject() = default;
    virtual btCollisionObject* getCollisionObject() const = 0;

    void createHandlerEntry();
    void removeHandlerEntry();

    std::shared_ptr<Event> onEnter = std::make_shared<Event>();
    std::shared_ptr<Event> onInside = std::make_shared<Event>();
    std::shared_ptr<Event> onExit = std::make_shared<Event>();

    std::shared_ptr<Event> getEnterEvent() { return onEnter; }
    std::shared_ptr<Event> getInsideEvent() { return onInside; }
    std::shared_ptr<Event> getExitEvent() { return onExit; }
};

void bindPhysicsObject();