#include "CharacterComponent.h"
#include "BodyComponent.h"
#include "Object.h"
#include "InputDevice.h"
#include "SpriteComponent.h"
#include "SDL.h"
#include "Engine.h"
#include <iostream>



void CharacterComponent::update() 
{
    float vx=0;
    float speed =  3.0f;
    float gravity = 0.3f;

    BodyComponent* body = getObject()->getComponent<BodyComponent>();
   // SpriteComponent* sprite = getObject()->getComponent<SpriteComponent>();

    if (!body) return;
    bool onGround = body->getY() >= .8*Engine::E->getHeight();
    if(InputDevice::isKeyDown(SDL_SCANCODE_A)) {
        std::cout << "'A' is Pressed! goingLeft" << std::endl;
        vx-=speed;

    }
    if(InputDevice::isKeyDown(SDL_SCANCODE_D)) {
        std::cout << "'D' is Pressed! going Right" << std::endl;
        vx+=speed;
        // body->setVelocity(body->getVelocity() + b2Vec2(1, 0));
    }
    if(onGround) 
    {
        body->setVy(0); 
        body->setY(Engine::E->getHeight()*.8);
    }
    if(InputDevice::isKeyDown(SDL_SCANCODE_SPACE) && onGround) {
        std::cout << "Space - JUMMP!!" << std::endl;
        body->setVy(-10);
    }


    body->setVx(vx);
    body->setVy(body->getVy()+gravity);
    
}

