#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include "Object.h"

struct Library {
    std::unordered_map<std::string, std::function<std::unique_ptr<Object>(const tinyxml2::XMLElement*)>> map;
    Library();
};