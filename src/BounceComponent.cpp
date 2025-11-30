#include "BounceComponent.h"
#include "BodyComponent.h"
#include "Engine.h"
#include <iostream>

BounceComponent::BounceComponent() {
}

void BounceComponent::update(float dt) {
    checkBounds();
}

void BounceComponent::checkBounds() {
    Object* obj = getObject();
    if (!obj) return;
    
    BodyComponent* body = obj->getComponent<BodyComponent>();
    if (!body) return;
    
    // Get screen dimensions
    int screenWidth = Engine::E->getWidth();
    int screenHeight = Engine::E->getHeight();
    
    // Get body dimensions
    float bodyX = body->getX();
    float bodyY = body->getY();
    float bodyWidth = body->getWidth();
    float bodyHeight = body->getHeight();
    
    // Check left edge (x < 0)
    if (bodyX < bodyWidth/2) {
        body->setX(bodyWidth/2);
        body->setVx(-body->getVx());
    }
    
    // Check right edge (x + width > screenWidth)
    if (bodyX + bodyWidth/2 > screenWidth) {
        body->setX(screenWidth - bodyWidth/2);
        body->setVx(-body->getVx());
    }
    
    // Check top edge (y < 0)
    if (bodyY < bodyHeight/2) {
        body->setY(bodyHeight/2);
        body->setVy(-body->getVy());
    }
    
    // Check bottom edge (y + height > screenHeight)
    if (bodyY + bodyHeight/2 > screenHeight ) {
        body->setY(screenHeight - bodyHeight/2);
        body->setVy(-body->getVy());
    }
}

