#include "MissileComponent.h"
#include "BodyComponent.h"
#include "Object.h"
#include <cmath>

MissileComponent::MissileComponent(Object* target) 
    : target(target) {
}

void MissileComponent::update() {
    Object* body = getObject();
    BodyComponent* bodyComp = body->getComponent<BodyComponent>();
    Object* target = getTarget();
    BodyComponent* targetComp = target->getComponent<BodyComponent>();
    float targetX = targetComp->getX();
    float targetY = targetComp->getY();
    float bodyX = bodyComp->getX();
    float bodyY = bodyComp->getY();

    float radianAngle = atan2(targetY-bodyY, targetX-bodyX);
    float angle = radianAngle * 180.0f / M_PI;

    float speed = 1.0f;
    float xSpeed = speed * cos(radianAngle);
    float ySpeed = speed * sin(radianAngle);

    bodyComp->setVx(xSpeed);
    bodyComp->setVy(ySpeed);


    bodyComp->setAngle(angle);
    if(hypot(targetX-bodyX, targetY-bodyY) < 5) {
        targetComp ->setX(rand() % 640);
        targetComp ->setY(rand() % 480);
        targetComp ->setVx(rand() % 3 - 1);
        targetComp ->setVy(rand() % 3 - 1);
    }
}

