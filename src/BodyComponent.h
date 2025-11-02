#pragma once
#include "Component.h"

class Input;

class BodyComponent : public Component {
public:
    float x = 0, y = 0;
    float vx = 0, vy = 0;
    float angle = 0;

    void update(float dt, const Input& input) override {
        x += vx * dt;
        y += vy * dt;
    }
    void draw(SDL_Renderer* renderer) override {}
};