#include "PhysicsComponent.h"
#include "BodyComponent.h"
#include "Object.h"
#include "Engine.h"
#include <cmath>

PhysicsComponent::PhysicsComponent(float gravityX, float gravityY)
    : gravityX(gravityX), gravityY(gravityY) {}

void PhysicsComponent::update() {
    Object* obj = getObject();
    if (!obj) return;

    BodyComponent* body = obj->getComponent<BodyComponent>();
    if (!body) return;

    // 1️⃣ Apply gravity
    body->setVx(body->getVx() + gravityX);
    body->setVy(body->getVy() + gravityY);

    // 2️⃣ Move object
    float newX = body->getX() + body->getVx();
    float newY = body->getY() + body->getVy();
    body->setX(newX);
    body->setY(newY);

    // 3️⃣ Ground collision
    float groundY = Engine::E->getGroundY() - body->getHeight() / 2.0f;
    if (body->getY() > groundY) {
        body->setY(groundY);
        body->setVy(0);
    }

    // 4️⃣ Check collision with other objects (e.g., crates)
    auto& objects = Engine::E->getObjects();
    for (auto& otherObj : objects) {
        if (otherObj.get() == obj) continue; // skip self
        BodyComponent* otherBody = otherObj->getComponent<BodyComponent>();
        if (!otherBody) continue;

        // Simple AABB collision
        bool overlapX = std::abs(body->getX() - otherBody->getX()) < (body->getWidth() + otherBody->getWidth()) / 2.0f;
        bool overlapY = std::abs(body->getY() - otherBody->getY()) < (body->getHeight() + otherBody->getHeight()) / 2.0f;

        if (overlapX && overlapY) {
            // Push out on Y axis
            if (body->getVy() > 0) { // falling
                body->setY(otherBody->getY() - (body->getHeight() + otherBody->getHeight()) / 2.0f);
                body->setVy(0);
            } else if (body->getVy() < 0) { // jumping
                body->setY(otherBody->getY() + (body->getHeight() + otherBody->getHeight()) / 2.0f);
                body->setVy(0);
            }
        }
    }

    // Optional: apply simple friction
    body->setVx(body->getVx() * 0.98f);
}
