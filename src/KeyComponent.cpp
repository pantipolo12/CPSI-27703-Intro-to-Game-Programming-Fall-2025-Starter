#include "KeyComponent.h"
#include "BodyComponent.h"
#include "CharacterComponent.h"
#include "Engine.h"
#include "InputDevice.h"
#include <SDL.h>
#include <cmath>

// Helper function to check if H key is pressed (check both InputDevice and SDL directly)
static bool isHKeyPressed() {
    // First check InputDevice
    if (InputDevice::isKeyDown(SDL_SCANCODE_H)) {
        return true;
    }
    // Fallback: check SDL keyboard state directly
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    return keystate[SDL_SCANCODE_H] != 0;
}

void KeyComponent::update(float dt) {
    Object* keyObj = getObject();
    if (!keyObj) return;
    
    BodyComponent* keyBody = keyObj->getComponent<BodyComponent>();
    if (!keyBody) return;
    
    // Get player
    Object* player = Engine::E->getPlayer();
    if (!player) return;
    
    BodyComponent* playerBody = player->getComponent<BodyComponent>();
    if (!playerBody) return;
    
    bool hKeyPressed = isHKeyPressed();
    
    // If key is picked up, check if H is still held
    if (isPickedUp) {
        // If H key is released, drop the key
        if (!hKeyPressed) {
            isPickedUp = false;
            this->player = nullptr;
            std::cout << "[KEY] Key dropped (H key released)" << std::endl;
            return;
        }
        
        // H is still held, keep key attached to player
        if (this->player) {
            playerBody = this->player->getComponent<BodyComponent>();
            if (playerBody) {
                float playerH = playerBody->getHeight();
                float keyH = keyBody->getHeight();
                // Position key relative to player (e.g., above player's head)
                float offsetX = 0;
                float offsetY = -playerH / 2 - keyH / 2 - 10; // Above player
                keyBody->setX(playerBody->getX() + offsetX);
                keyBody->setY(playerBody->getY() + offsetY);
                keyBody->setVx(0);
                keyBody->setVy(0);
            }
        }
        return;
    }
    
    // Key is not picked up, check if player can pick it up
    // Check if player is in contact with key
    float keyX = keyBody->getX();
    float keyY = keyBody->getY();
    float keyW = keyBody->getWidth();
    float keyH = keyBody->getHeight();
    
    float playerX = playerBody->getX();
    float playerY = playerBody->getY();
    float playerW = playerBody->getWidth();
    float playerH = playerBody->getHeight();
    
    // Calculate bounds
    float keyLeft = keyX - keyW / 2;
    float keyRight = keyX + keyW / 2;
    float keyTop = keyY - keyH / 2;
    float keyBottom = keyY + keyH / 2;
    
    float playerLeft = playerX - playerW / 2;
    float playerRight = playerX + playerW / 2;
    float playerTop = playerY - playerH / 2;
    float playerBottom = playerY + playerH / 2;
    
    // Check for overlap
    bool overlapX = (playerRight > keyLeft) && (playerLeft < keyRight);
    bool overlapY = (playerBottom > keyTop) && (playerTop < keyBottom);
    bool inContact = overlapX && overlapY;
    
    // If in contact and H key is held, pick up the key
    if (inContact && hKeyPressed) {
        isPickedUp = true;
        
        // Store reference to player
        this->player = player;
        
        std::cout << "[KEY] Key picked up by player! (Hold H to keep it)" << std::endl;
    }
}

bool KeyComponent::isPickedUpByPlayer() const {
    return isPickedUp;
}

