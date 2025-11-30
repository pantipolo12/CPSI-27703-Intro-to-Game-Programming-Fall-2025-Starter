#pragma once
#include <string>
#include "Engine.h"

class LevelLoader {
public:
    static bool load(const std::string& filename, Engine& engine);
};
