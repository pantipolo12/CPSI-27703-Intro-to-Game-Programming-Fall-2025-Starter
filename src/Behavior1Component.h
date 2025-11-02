#pragma once
#include "Component.h"
#include "BodyComponent.h"

class Behavior1Component : public Component {
public:
    float speed = 2.0f;    // world units per second
    float direction = 1.0f; // 1 = right, -1 = left
    float minX = 2.0f;
    float maxX = 6.0f;

    void update(float dt, const Input& input) override {
        BodyComponent* body = parent().get<BodyComponent>();
        if (!body) return;

        body->x += speed * direction * dt;

        // Reverse direction at bounds
        if (body->x > maxX) direction = -1.0f;
        if (body->x < minX) direction = 1.0f;
    }

    void draw(SDL_Renderer* renderer) override {}
};
