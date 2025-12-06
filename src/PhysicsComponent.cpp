#include "PhysicsComponent.h"
#include "BodyComponent.h"
#include "Object.h"
#include "Engine.h"
#include "GroundComponent.h"
#include <cmath>

PhysicsComponent::PhysicsComponent(float gravityX, float gravityY)
    : gravityX(gravityX), gravityY(gravityY) {}

void PhysicsComponent::update(float dt) {
    Object* obj = getObject();
    if (!obj) return;

    BodyComponent* body = obj->getComponent<BodyComponent>();
    if (!body) return;

    // Apply gravity
    body->setVy(body->getVy() + gravityY);
    body->setVx(body->getVx() + gravityX);

    float newX = body->getX() + body->getVx();
    float newY = body->getY() + body->getVy();

    //Get all objects in the world
    auto& objects = Engine::E->getObjects();

    for (auto& otherObj : objects) {
        if (otherObj.get() == obj) continue; // skip self

        // Only check objects with BodyComponent
        BodyComponent* otherBody = otherObj->getComponent<BodyComponent>();
        if (!otherBody) continue;

        // Only collide with ground
        if (!otherObj->getComponent<GroundComponent>()) continue;

        // AABB collision detection
        bool overlapX = newX + body->getWidth() > otherBody->getX() &&
                        newX < otherBody->getX() + otherBody->getWidth();
        bool overlapY = newY + body->getHeight() > otherBody->getY() &&
                        newY < otherBody->getY() + otherBody->getHeight();

        if (overlapX && overlapY) {
            // Only fix vertical collisions
            if (body->getVy() > 0) { // falling
                newY = otherBody->getY() - body->getHeight();
                body->setVy(0);
            } else if (body->getVy() < 0) { // jumping
                newY = otherBody->getY() + otherBody->getHeight();
                body->setVy(0);
            }
        }
    }

    // Apply final positions
    body->setX(newX);
    body->setY(newY);

    // Optional: horizontal friction
    body->setVx(body->getVx() * 0.98f);
}
