#pragma once
#include "Component.h"

class PhysicsComponent : public Component {
public:
    PhysicsComponent(float gravityX = 0.0f, float gravityY = 1.0f);
    
    // Getters
    float getGravityX() const { return gravityX; }
    float getGravityY() const { return gravityY; }
    
    // Setters
    void setGravityX(float newGravityX) { gravityX = newGravityX; }
    void setGravityY(float newGravityY) { gravityY = newGravityY; }
    
    void update() override;

private:
    float gravityX;
    float gravityY;
};

