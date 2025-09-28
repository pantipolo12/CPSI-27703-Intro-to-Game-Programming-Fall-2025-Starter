#pragma once
#include "Object.h"

struct Crate : public Object {
    static std::unique_ptr<Object> create(const tinyxml2::XMLElement* element);
    void update() override;
    void draw(SDL_Renderer*) override;
};