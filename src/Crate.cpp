#include "Crate.h"
#include <iostream>
#include "Utils.h"

std::unique_ptr<Object> Crate::create(const tinyxml2::XMLElement* element) {
    auto obj = std::make_unique<Crate>();
    const tinyxml2::XMLElement* body = element->FirstChildElement("Body");
    if (body) {
        body->QueryFloatAttribute("x", &obj->x);
        body->QueryFloatAttribute("y", &obj->y);
        body->QueryFloatAttribute("angle", &obj->angle);
    }
    return obj;
}

void Crate::update() {
    std::cout << "[Crate] Updating...\n";
}

void Crate::draw(SDL_Renderer* renderer) {
    SDL_FPoint pos = worldToScreen(x,y);

    SDL_Rect rect;
    rect.w = rect.h = 64;
    rect.x = static_cast<int>(pos.x - rect.w / 2);
    rect.y = static_cast<int>(pos.y - rect.h);

    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
    SDL_RenderFillRect(renderer, &rect);

    std::cout << "[Crate] Drawing at (" << x << ", " << y << ")  angle: " << angle << "\n";
}