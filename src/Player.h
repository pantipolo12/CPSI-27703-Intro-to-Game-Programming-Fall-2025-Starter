#pragma once
#include "Object.h"
#include <SDL.h>

struct Player : public Object {
    static std::unique_ptr<Object> create(const tinyxml2::XMLElement* element);
    void update() override;
    void draw(SDL_Renderer* renderer) override;
};