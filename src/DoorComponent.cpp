#include "DoorComponent.h"
#include "BodyComponent.h"
#include "KeyComponent.h"
#include "CharacterComponent.h"
#include "Engine.h"
#include "LevelLoader.h"
#include <SDL.h>
#include <cmath>
#include <algorithm>

void DoorComponent::update(float dt) {
    if (isOpen) return; // Door already opened, don't check again
    
    Object* doorObj = getObject();
    if (!doorObj) {
        std::cout << "[DOOR] ERROR: Door object is null!" << std::endl;
        return;
    }
    
    BodyComponent* doorBody = doorObj->getComponent<BodyComponent>();
    if (!doorBody) {
        std::cout << "[DOOR] ERROR: Door has no BodyComponent!" << std::endl;
        return;
    }
    
    // Get player
    Object* player = Engine::E->getPlayer();
    if (!player) {
        std::cout << "[DOOR] ERROR: Player is null!" << std::endl;
        return;
    }
    
    BodyComponent* playerBody = player->getComponent<BodyComponent>();
    if (!playerBody) {
        std::cout << "[DOOR] ERROR: Player has no BodyComponent!" << std::endl;
        return;
    }
    
    // Check if player has key
    bool hasKey = false;
    for (auto& obj : Engine::E->getObjects()) {
        KeyComponent* keyComp = obj->getComponent<KeyComponent>();
        if (keyComp && keyComp->isPickedUpByPlayer()) {
            hasKey = true;
            break;
        }
    }
    
    if (!hasKey) {
        return; // Player doesn't have key, can't open door
    }
    
    // Check if player is aligned with door (at door's x and y coordinates)
    float doorX = doorBody->getX();
    float doorY = doorBody->getY();
    float doorW = doorBody->getWidth();
    float doorH = doorBody->getHeight();
    
    float playerX = playerBody->getX();
    float playerY = playerBody->getY();
    float playerW = playerBody->getWidth();
    float playerH = playerBody->getHeight();
    
    // Calculate alignment tolerance
    float toleranceX = (doorW + playerW) / 2 + 20; // Allow some overlap
    float toleranceY = (doorH + playerH) / 2 + 20;
    
    float distanceX = std::abs(playerX - doorX);
    float distanceY = std::abs(playerY - doorY);
    
    // Check if player is aligned with door (use collision detection instead of just position)
    // Check for actual overlap between player and door
    float playerLeft = playerX - playerW / 2;
    float playerRight = playerX + playerW / 2;
    float playerTop = playerY - playerH / 2;
    float playerBottom = playerY + playerH / 2;
    
    float doorLeft = doorX - doorW / 2;
    float doorRight = doorX + doorW / 2;
    float doorTop = doorY - doorH / 2;
    float doorBottom = doorY + doorH / 2;
    
    // Check for overlap (with larger tolerance for easier collision)
    float tolerance = 30.0f; // Extra tolerance for easier collision detection
    bool overlapX = (playerRight + tolerance > doorLeft) && (playerLeft - tolerance < doorRight);
    bool overlapY = (playerBottom + tolerance > doorTop) && (playerTop - tolerance < doorBottom);
    bool inContact = overlapX && overlapY;
    
    if (inContact && !isOpen && hasKey) {
        // Player is in contact with door and has key - open door and transition to next level
        isOpen = true;
        std::cout << "[DOOR] ========================================" << std::endl;
        std::cout << "[DOOR] *** DOOR OPENED! ***" << std::endl;
        std::cout << "[DOOR] Player has key and is in contact!" << std::endl;
        std::cout << "[DOOR] Transitioning to: " << nextLevel << std::endl;
        std::cout << "[DOOR] ========================================" << std::endl;
        
        // Queue level load for next frame to avoid crashes (don't load during update)
        Engine::E->queueLevelLoad(nextLevel);
    }
}

