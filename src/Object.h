#pragma once
#include <tinyxml2.h>
#include <memory>
#include <SDL.h>



struct Object {
    float x{}, y{}, angle{};
    virtual ~Object() = default;
    virtual void update() = 0;
    virtual void draw(SDL_Renderer* renderer) = 0;
};