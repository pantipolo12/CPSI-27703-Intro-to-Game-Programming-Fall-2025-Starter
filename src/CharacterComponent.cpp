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
    // Player can stand on objects with GroundComponent OR any solid object (like crates)
    bool onGround = false;
    Object* standingOnObject = nullptr;

    for (auto& obj : Engine::E->getObjects()) {
        // Skip the player itself
        if (obj.get() == getObject()) continue;

        auto* gBody = obj->getComponent<BodyComponent>();
        if (!gBody) continue;

        // Check if this object has GroundComponent OR is a solid object (like a crate)
        auto ground = obj->getComponent<GroundComponent>();
        bool isSolidObject = (ground != nullptr) || (gBody != nullptr); // Any object with BodyComponent can be stood on
        
        if (!isSolidObject) continue;

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
        // For crates and other objects, we need to be more lenient with the distance check
        float checkDistance = ground ? groundCheckDistance : groundCheckDistance * 1.5f; // More lenient for non-ground objects
        
        if (overlapX && distanceToGround >= -checkDistance && distanceToGround <= checkDistance && vy >= -0.5f) {
            // Make sure the object is actually below the player (player's bottom is near object's top)
            if (playerBottom >= groundTop - 5.0f && playerBottom <= groundTop + checkDistance) {
                onGround = true;
                standingOnObject = obj.get();
                
                // If player has penetrated ground or is very close, snap to ground surface
                // Player bottom should be exactly at ground top (not sinking)
                // Only do this for GroundComponent objects to avoid interfering with crate physics
                if (ground && distanceToGround > -2.0f && distanceToGround < 10.0f) {
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
    
    // Update flip state based on input (not just movement)
    // This way the flip persists even when player stops
    if (leftPressed) {
        lastFlip = SDL_FLIP_HORIZONTAL; // Facing left
    } else if (rightPressed) {
        lastFlip = SDL_FLIP_NONE; // Facing right
    }
    // If neither key is pressed, keep the last flip state (don't reset it)
    
    if (animate && sprite) {
        // Always apply the flip state to both components
        animate->setFlip(lastFlip);
        sprite->setFlip(lastFlip);
        
        if (isMoving) {
            // Walking: switch to walk animation
            if (animate->getTextureName() != "playerGIGIwalk6") {
                animate->setTexture("playerGIGIwalk6", 6, 64, 64, 10);
            }
            animate->setEnabled(true);
            sprite->setEnabled(false);
        } else {
            // Idle: switch to idle animation
            if (animate->getTextureName() != "playerGIGIIdle") {
                animate->setTexture("playerGIGIIdle", 6, 64, 64, 10);
            }
            // Always enable animation when idle (it should be animating)
            animate->setEnabled(true);
            sprite->setEnabled(false);
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
