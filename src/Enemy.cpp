#include "Enemy.h"
#include <iostream>
#include <SDL.h>
#include "Utils.h"

std::unique_ptr<Object> Enemy::create(const tinyxml2::XMLElement* element) {
    auto obj = std::make_unique<Enemy>();
    const tinyxml2::XMLElement* body = element->FirstChildElement("Body");
    if (body) {
        body->QueryFloatAttribute("x", &obj->x);
        body->QueryFloatAttribute("y", &obj->y);
        body->QueryFloatAttribute("angle", &obj->angle);
    }
    return obj;
}

void Enemy::update() {
    std::cout << "[Enemy] Updating...\n";
}

void Enemy::draw(SDL_Renderer* renderer) {
    SDL_FPoint pos = worldToScreen(x, y);

    SDL_Rect rect;
    rect.w = rect.h = 64; // enemy size
    rect.x = static_cast<int>(pos.x - rect.w / 2);
    rect.y = static_cast<int>(pos.y - rect.h);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);

    std::cout << "[Player] Drawing at (" << x << ", " << y << ") angle: " << angle << "\n";
}