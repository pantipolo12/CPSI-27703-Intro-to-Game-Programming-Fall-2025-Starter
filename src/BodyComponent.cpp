#include "BodyComponent.h"

BodyComponent::BodyComponent(float x, float y, float width, float height, float angle, float vx, float vy) 
    : x(x), y(y), width(width), height(height), angle(angle), vx(vx), vy(vy) {
}

void BodyComponent::update(float dt) {
    // Simple movement update
    x += vx;
    y += vy;

    // Future: add collision, gravity, or constraints here
}


SDL_Rect BodyComponent::getRect() const {
    SDL_Rect rect;
    rect.x = static_cast<int>(x);
    rect.y = static_cast<int>(y);
    
    rect.w = static_cast<int>(width);
    rect.h = static_cast<int>(height);
    return rect;

}