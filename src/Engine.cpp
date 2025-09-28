#include "Engine.h"
#include <tinyxml2.h>
#include <iostream>
#include <SDL.h>

Engine::Engine(const std::string& levelPath) {
    loadLevel(levelPath);
}

void Engine::loadLevel(const std::string& levelPath) {
    using namespace tinyxml2;
    XMLDocument doc;
    if (doc.LoadFile(levelPath.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load level file: " << levelPath << "\n";
        return;
    }

    XMLElement* level = doc.FirstChildElement("Level");
    if (!level) return;

    for (XMLElement* elem = level->FirstChildElement("Object"); elem != nullptr; elem = elem->NextSiblingElement("Object")) {
        const char* type = elem->Attribute("type");
        if (type && objectLibrary.map.count(type)) {
            auto obj = objectLibrary.map[type](elem);
            objects.push_back(std::move(obj));
        } else {
            std::cerr << "Unknown type: " << (type ? type : "nullptr") << "\n";
        }
    }
}

void Engine::update() {
    for (auto& obj : objects)
        obj->update();
}

void Engine::draw(SDL_Renderer* renderer) {
    for (auto& obj : objects)
        obj->draw(renderer);
}
