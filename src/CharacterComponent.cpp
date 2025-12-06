#include <SDL.h>
#include "CharacterComponent.h"
#include "BodyComponent.h"
#include "Object.h"
#include "Engine.h"
#include "GroundComponent.h"
#include "InputDevice.h"
#include "AnimateComponent.h"
#include <iostream>
#include <cmath>

// include for debug
#include "SpriteComponent.h"

void CharacterComponent::update(float dt) {
    auto* body = getObject()->getComponent<BodyComponent>();
    if (!body) return;

    // Read current body state from Box2D
    float px = body->getX();
    float py = body->getY();
    float pw = body->getWidth();
    float ph = body->getHeight();

    float vx = body->getVx();
    float vy = body->getVy();

    // Constants
    const float speed = 200.0f;  // Increased speed for faster movement
    const float jumpForce = -500.0f;  // Strong jump force to overcome gravity (negative = up in SDL coordinates)
    const float groundCheckDistance = 50.0f;

    // Calculate player bounds (center-based coordinates)
    float playerLeft = px - pw / 2;
    float playerRight = px + pw / 2;
    float playerTop = py - ph / 2;
    float playerBottom = py + ph / 2;

    // --- Check if on ground ---
    bool onGround = false;

    for (auto& obj : Engine::E->getObjects()) {
        auto ground = obj->getComponent<GroundComponent>();
        if (!ground) continue;

        auto* gBody = obj->getComponent<BodyComponent>();
        if (!gBody) continue;

        float gx = gBody->getX();
        float gy = gBody->getY();
        float gw = gBody->getWidth();
        float gh = gBody->getHeight();

        float groundLeft = gx - gw / 2;
        float groundRight = gx + gw / 2;
        float groundTop = gy - gh / 2;
        float groundBottom = gy + gh / 2;

        // Horizontal overlap
        bool overlapX = (playerRight > groundLeft) && (playerLeft < groundRight);

        // Distance to ground top (positive = player below ground, negative = player above ground)
        float distanceToGround = playerBottom - groundTop;

        // Check if player is on or very close to ground
        if (overlapX && distanceToGround >= -groundCheckDistance && distanceToGround <= groundCheckDistance && vy >= -0.5f) {
            onGround = true;
            
            // If player has penetrated ground or is very close, snap to ground surface
            // Player bottom should be exactly at ground top (not sinking)
            if (distanceToGround > -2.0f && distanceToGround < 10.0f) {
                // Set player center so that player bottom = ground top
                // playerBottom = py + ph/2 = groundTop
                // Therefore: py = groundTop - ph/2
                float targetY = groundTop - ph / 2;
                float currentY = body->getY();
                
                // Only adjust if we're actually penetrating or very close
                if (std::abs(currentY - targetY) > 1.0f) {
                    body->setY(targetY);
                    body->setVy(0);
                }
            }
        }
    }

    // --- Input handling ---
    float newVx = 0;
    bool leftPressed = InputDevice::isKeyDown(SDL_SCANCODE_A);
    bool rightPressed = InputDevice::isKeyDown(SDL_SCANCODE_D);
    bool jumpPressed = InputDevice::isKeyDown(SDL_SCANCODE_SPACE);

    if (leftPressed) {
        newVx -= speed;
        std::cout << "[INPUT] A pressed: moving left\n";
    }
    if (rightPressed) {
        newVx += speed;
        std::cout << "[INPUT] D pressed: moving right\n";
    }
    if (jumpPressed && onGround) {
        body->setVy(jumpForce);
        std::cout << "[INPUT] SPACE pressed: jump applied\n";
    }

    // Only update horizontal velocity if changed
    if (newVx != vx) {
        body->setVx(newVx);
        std::cout << "[STATE] Horizontal velocity changed: Vx=" << newVx << "\n";
    }
    
    // --- Animation control ---
    // Get AnimateComponent and SpriteComponent
    auto* animate = getObject()->getComponent<AnimateComponent>();
    auto* sprite = getObject()->getComponent<SpriteComponent>();
    
    // Check if player is moving horizontally
    bool isMoving = std::abs(newVx) > 0.1f;
    
    if (animate && sprite) {
        if (isMoving) {
            // Enable animation, disable static sprite
            animate->setEnabled(true);
            sprite->setEnabled(false);
            
            // Set flip based on direction
            if (newVx < 0) {
                animate->setFlip(SDL_FLIP_HORIZONTAL); // Moving left
            } else if (newVx > 0) {
                animate->setFlip(SDL_FLIP_NONE); // Moving right
            }
        } else {
            // Disable animation, enable static sprite
            animate->setEnabled(false);
            sprite->setEnabled(true);
        }
    }

    // --- Clamp horizontal position to world bounds ---
    float finalX = body->getX();
    float halfWidth = pw / 2;
    if (finalX - halfWidth < 0) {
        body->setX(halfWidth);
        body->setVx(0);
        std::cout << "[CLAMP] Clamped to left world boundary\n";
    }
    if (finalX + halfWidth > Engine::E->getView().worldWidth) {
        body->setX(Engine::E->getView().worldWidth - halfWidth);
        body->setVx(0);
        std::cout << "[CLAMP] Clamped to right world boundary\n";
    }

    // At the end of CharacterComponent::update(), after all movement/physics
    auto* obj = getObject(); // current object
    SDL_Rect rect = body->getRect();
    

    std::cout << "[DEBUG] Object: " << "PlayerGIGI"
              << ", X: " << rect.x
              << ", Y: " << rect.y
              << ", W: " << rect.w
              << ", H: " << rect.h
              << ", Vx: " << body->getVx()
              << ", Vy: " << body->getVy()
              << ", OnGround: " << (onGround ? "Yes" : "No") 
              << "\n";
    

    // --- Debug final state ---
    std::cout << "[DEBUG] Position: (" << body->getX() << ", " << body->getY()
              << ")  Velocity: (" << body->getVx() << ", " << body->getVy()
              << ")  OnGround: " << (onGround ? "Yes" : "No") << "\n";
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