#include "Player.h"
#include <iostream>
#include <SDL.h>
#include "Utils.h"


std::unique_ptr<Object> Player::create(const tinyxml2::XMLElement* element) {
    auto obj = std::make_unique<Player>();
    const tinyxml2::XMLElement* body = element->FirstChildElement("Body");
    if (body) {
        body->QueryFloatAttribute("x", &obj->x);
        body->QueryFloatAttribute("y", &obj->y);
        body->QueryFloatAttribute("angle", &obj->angle);
    }
    return obj;
}

void Player::update() {
    std::cout << "[Player] Updating...\n";
}

void Player::draw(SDL_Renderer* renderer) {
    SDL_FPoint pos = worldToScreen(x, y);

    SDL_Rect rect;
    rect.w = rect.h = 64; // size of your player
    rect.x = static_cast<int>(pos.x - rect.w / 2);
    rect.y = static_cast<int>(pos.y - rect.h);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green
    SDL_RenderFillRect(renderer, &rect);

    std::cout << "[Player] Drawing at (" << x << ", " << y << ") angle: " << angle << "\n";
}
