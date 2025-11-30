#include <SDL.h>
#include "CharacterComponent.h"
#include "BodyComponent.h"
#include "Object.h"
#include "Engine.h"
#include "GroundComponent.h"
#include "InputDevice.h"
#include <iostream>

void CharacterComponent::update(float dt) {
    auto* body = getObject()->getComponent<BodyComponent>();
    if (!body) return;

    // Read current body state
    float px = body->getX();
    float py = body->getY();
    float pw = body->getWidth();
    float ph = body->getHeight();

    float vx = body->getVx();
    float vy = body->getVy();
    
    // Constants
    const float speed = 3.0f;
    const float gravity = 0.3f;
    const float jumpForce = -10.0f;
    
    // Horizontal Movement
    vx = 0;
    if (InputDevice::isKeyDown(SDL_SCANCODE_A)) vx -= speed;
    if (InputDevice::isKeyDown(SDL_SCANCODE_D)) vx += speed;

    // apply gravity
    vy += gravity;

    // Predict next position
    float nextX = px + vx;
    float nextY = py + vy;

    bool onGround = false;

    // Check collision with all ground objects
    for (auto& obj : Engine::E->getObjects()) {
        auto ground = obj->getComponent<GroundComponent>();
        if (!ground) continue;
    
        auto* gBody = obj->getComponent<BodyComponent>();
        if (!gBody) continue;
    
        std::cout << "Ground found: id=" << obj->getId() << " y=" << gBody->getY() << std::endl;
    
        float gx = gBody->getX();
        float gy = gBody->getY();
        float gw = gBody->getWidth();
        float gh = gBody->getHeight();
    
        //Che3ck horizontal overlap
        bool overlapX = (px + pw > gx) && (px < gx + gw);

        //check if falling onto ground
        bool fallingOntoGround = (py + ph <= gy) // player feet above ground
         && (nextY + ph >= gy);  // next frame feet cross ground

        if(overlapX && fallingOntoGround) {
            //snap on top
            py = gy - ph;
            vy = 0;
            onGround = true;
        }
    }
    
    // Jumping
    if (onGround && InputDevice::isKeyDown(SDL_SCANCODE_SPACE)) {
        vy = jumpForce;
        onGround = false;
    }

    // Clamp to world bounds
    if (nextX < 0) nextX = 0;
    if (nextX + pw > Engine::E->getView().worldWidth) nextX = Engine::E->getView().worldWidth - pw;
    
    // nextY is NOT used because py was corrected
    if(py < 0) py = 0;

    // apply movement
    body->setX(nextX);
    body->setY(py);
    body->setVx(vx);
    body->setVy(vy);

}

// void CharacterComponent::drawDebug(SDL_Renderer* renderer) {
//             auto*  body = getObject()->getComponent<BodyComponent>();
//             if(!body) return;

//             //character
//             SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
//             SDL_Rect charRect = body->getRect();
//             SDL_RenderDrawRect(renderer, &charRect);

//             //ground
//             for (auto* obj : Engine::E->getObjects())
//             {
//                 auto* gBody = obj->getComponent<BodyComponent>();
//                 auto* ground = obj->getComponent<GroundComponent>();
//                 if(!gBody || !ground) continue;

//                 SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
//                 SDL_Rect groundRect = gBody->getRect();
//                 SDL_RenderDrawRect(renderer, &groundRect);
//             }

//     }