#pragma once
#include "Object.h"
#include "Library.h"
#include <vector>
#include <memory>
#include <string>
#include <SDL.h>

struct Engine {
    Library objectLibrary;
    std::vector<std::unique_ptr<Object>> objects;

    Engine() = default;
    Engine(const std::string& levelPath);

    void loadLevel(const std::string& levelPath);
    void update();
    void draw(SDL_Renderer*);
};
