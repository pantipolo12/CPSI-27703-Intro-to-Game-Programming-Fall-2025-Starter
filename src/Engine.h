#pragma once
#include <vector>
#include <memory>
#include <string>
#include <cstdio>
#include <SDL.h>
#include <tinyxml2.h>
#include "GameObject.h"
#include "ComponentFactory.h"

class Input;

class Engine {
    private:
        std::vector<std::unique_ptr<GameObject>> objects;
        SDL_Renderer* renderer;
    
    public:
        Engine(const std::string& levelPath, SDL_Renderer* rend) : renderer(rend) {
            loadLevel(levelPath);
        }
    
        void loadLevel(const std::string& levelPath) {
            using namespace tinyxml2;
            XMLDocument doc;
            if (doc.LoadFile(levelPath.c_str()) != XML_SUCCESS) {
                SDL_Log("Failed to load level: %s", levelPath.c_str());
                return;
            }
    
            XMLElement* level = doc.FirstChildElement("Level");
            if (!level) {
                printf("[Engine] No Level element found\n");
                return;
            }

            ComponentFactory factory(renderer);

            int objectCount = 0;
            for (XMLElement* elem = level->FirstChildElement("GameObject"); elem; elem = elem->NextSiblingElement("GameObject")) {
                auto obj = std::make_unique<GameObject>();
                int compCount = 0;

                for (XMLElement* compElem = elem->FirstChildElement(); compElem; compElem = compElem->NextSiblingElement()) {
                    factory.createComponent(obj.get(), compElem);
                    compCount++;
                }

                objects.push_back(std::move(obj));
                objectCount++;
                printf("[Engine] Loaded GameObject with %d components\n", compCount);
            }
            printf("[Engine] Total objects loaded: %d\n", objectCount);
        }
    
        void update(const Input& input, float dt = 0.016f) {
            for (auto& obj : objects) obj->update(dt, input);
        }
    
        void draw(SDL_Renderer* renderer) {
            for (auto& obj : objects) obj->draw(renderer);
        }
    };