#include "MissileComponent.h"
#include "BodyComponent.h"
#include "Object.h"
#include <cmath>
#include <box2d/box2d.h>

MissileComponent::MissileComponent(Object* target) 
    : target(target) {
}

void MissileComponent::update(float dt) {
    if (!target) return;
    
    Object* body = getObject();
    BodyComponent* bodyComp = body->getComponent<BodyComponent>();
    if (!bodyComp) return;
    
    BodyComponent* targetComp = target->getComponent<BodyComponent>();
    if (!targetComp) return;
    
    float targetX = targetComp->getX();
    float targetY = targetComp->getY();
    float bodyX = bodyComp->getX();
    float bodyY = bodyComp->getY();

    // Calculate direction to target
    float dx = targetX - bodyX;
    float dy = targetY - bodyY;
    float distance = sqrt(dx * dx + dy * dy);
    
    // If too close, don't move
    if (distance < 5.0f) {
        bodyComp->setVx(0);
        bodyComp->setVy(0);
        return;
    }
    
    // Normalize direction
    float dirX = dx / distance;
    float dirY = dy / distance;
    
    // Set speed (increased for better following)
    float speed = 150.0f; // Increased from 1.0f to 150.0f for visible movement
    
    // Set velocity towards target
    bodyComp->setVx(dirX * speed);
    bodyComp->setVy(dirY * speed);
    
    // Calculate and set angle
    float radianAngle = atan2(dy, dx);
    float angle = radianAngle * 180.0f / 3.14159265f;
    bodyComp->setAngle(angle);
}
