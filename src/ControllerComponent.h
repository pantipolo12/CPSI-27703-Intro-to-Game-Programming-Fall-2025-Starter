#pragma once
#include "Component.h"
#include "Input.h"
#include "GameObject.h"
#include "BodyComponent.h" // assumes you have this component

class ControllerComponent : public Component {
private:
    float speed = 200.0f;

public:
    void setSpeed(float s) { speed = s; }
    float getSpeed() const { return speed; }

    void update(float dt, const Input& input) override;
    void draw(SDL_Renderer* renderer) override {}
};
