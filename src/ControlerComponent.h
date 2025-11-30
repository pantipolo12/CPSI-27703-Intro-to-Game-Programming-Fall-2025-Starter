#pragma once
#include "Component.h"
#include "InputDevice.h"
#include "BodyComponent.h"
#include "Object.h"
#include <SDL.h>

// ControllerComponent allows player-controlled movement
class ControllerComponent : public Component {
public:
    float speed = 200.0f;   // horizontal speed in pixels/sec
    bool canJump = false;    // whether the player can jump

    void update() override {
        // Get the BodyComponent of this GameObject
        BodyComponent* body = object ? object->getComponent<BodyComponent>() : nullptr;
        if (!body) return;

        const float SCALE = 64.0f; // pixels per world unit
        float worldSpeed = speed / SCALE; // convert pixels/sec to Box2D units/sec

        // Horizontal movement
        if (InputDevice::isKeyDown(SDL_SCANCODE_A)) {
            body->setVx(-worldSpeed);
        } else if (InputDevice::isKeyDown(SDL_SCANCODE_D)) {
            body->setVx(worldSpeed);
        } else {
            body->setVx(0);
        }

        // Jump (simple version)
        if (body->getY() <= 1.01f) canJump = true; // on ground
        if (InputDevice::isKeyDown(SDL_SCANCODE_SPACE) && canJump) {
            body->setVy(5.0f); // jump velocity
            canJump = false;
        }
    }

    void render() override {
        // Controller does not draw anything
    }
};

