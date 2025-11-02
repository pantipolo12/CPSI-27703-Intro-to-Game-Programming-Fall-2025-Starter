#include "ControllerComponent.h"
#include "BodyComponent.h"
#include "Input.h"
#include <SDL.h>
#include <cmath> // for std::sqrt

void ControllerComponent::update(float dt, const Input& input) {
    GameObject& parentObj = parent();
    BodyComponent* body = parentObj.get<BodyComponent>();
    if (!body) return;

    float moveX = 0.0f;
    float moveY = 0.0f;

    // Check keys using the instance method
    if (input.isKeyDown(SDL_SCANCODE_W)) moveY += 1;
    if (input.isKeyDown(SDL_SCANCODE_S)) moveY -= 1;
    if (input.isKeyDown(SDL_SCANCODE_A)) moveX -= 1;
    if (input.isKeyDown(SDL_SCANCODE_D)) moveX += 1;

    // Normalize diagonal movement
    float length = std::sqrt(moveX * moveX + moveY * moveY);
    if (length > 0) {
        moveX /= length;
        moveY /= length;
    }

    // Update position directly using speed and delta time
    body->x += moveX * speed * dt;
    body->y += moveY * speed * dt;
}
