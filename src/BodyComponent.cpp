#include "BodyComponent.h"

BodyComponent::BodyComponent(float x, float y, float width, float height, float angle, float vx, float vy) 
    : x(x), y(y), width(width), height(height), angle(angle), vx(vx), vy(vy) {
}

void BodyComponent::update() {
    // Simple movement update
    x += vx;
    y += vy;

    // Future: add collision, gravity, or constraints here
}
