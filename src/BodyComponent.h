#pragma once
#include "Component.h"
#include <string>

class BodyComponent : public Component {
public:
    BodyComponent(float x=0, float y=0, float width=100, float height=100, float angle=0, float vx=0, float vy=0);
    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getVx() const { return vx; }
    float getVy() const { return vy; }
    float getAngle() const { return angle; }
    // Setters
    void setX(float newX) { x = newX; }
    void setY(float newY) { y = newY; }
    void setWidth(float newWidth) { width = newWidth; }
    void setHeight(float newHeight) { height = newHeight; }
    void setVx(float newVx) { vx = newVx; }
    void setVy(float newVy) { vy = newVy; }
    void setAngle(float newAngle) { angle = newAngle; }

    void update() override;
    // update() and render() don't need to be overridden - using defaults from Component

private:
    float x;
    float y;
    float vx;
    float vy;
    float angle;
    float width;
    float height;
};
