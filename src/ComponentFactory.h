#pragma once
#include <string>
#include <map>
#include <functional>
#include <tinyxml2.h>
#include "GameObject.h"
#include "BodyComponent.h"
#include "SpriteComponent.h"
#include "ControllerComponent.h"
#include "Behavior1Component.h"
#include <SDL.h>
#include <SDL_image.h>

class ComponentFactory {
public:
    using CreateFunc = std::function<void(GameObject*, const tinyxml2::XMLElement*)>;
    std::map<std::string, CreateFunc> map;
    SDL_Renderer* renderer; // need this to create textures

    ComponentFactory(SDL_Renderer* rend) : renderer(rend) {
        // ---------------- BodyComponent ----------------
        map["BodyComponent"] = [](GameObject* obj, const tinyxml2::XMLElement* elem) {
            BodyComponent* body = obj->add<BodyComponent>();
            elem->QueryFloatAttribute("x", &body->x);
            elem->QueryFloatAttribute("y", &body->y);
            elem->QueryFloatAttribute("angle", &body->angle);
        };

        // ---------------- SpriteComponent ----------------
        map["SpriteComponent"] = [this](GameObject* obj, const tinyxml2::XMLElement* elem) {
            SpriteComponent* sprite = obj->add<SpriteComponent>();

            // Width/Height
            int w = 64, h = 64;
            elem->QueryIntAttribute("width", &w);
            elem->QueryIntAttribute("height", &h);
            sprite->width = w;
            sprite->height = h;

            // Color
            int r = 255, g = 255, b = 255;
            elem->QueryIntAttribute("r", &r);
            elem->QueryIntAttribute("g", &g);
            elem->QueryIntAttribute("b", &b);
            sprite->color = { (Uint8)r, (Uint8)g, (Uint8)b, 255 };

            // Image loading
            const char* imgPath = elem->Attribute("image");
            if (imgPath) {
                sprite->texturePath = imgPath;
                SDL_Surface* surface = IMG_Load(imgPath);
                if (!surface) {
                    SDL_Log("Failed to load image %s: %s", imgPath, IMG_GetError());
                } else {
                    sprite->texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_FreeSurface(surface);
                    if (!sprite->texture) {
                        SDL_Log("Failed to create texture from %s: %s", imgPath, SDL_GetError());
                    } else {
                        SDL_Log("Loaded image %s successfully", imgPath);
                    }
                }
            }
        };

        // ---------------- ControllerComponent ----------------
        map["ControllerComponent"] = [](GameObject* obj, const tinyxml2::XMLElement* elem) {
            ControllerComponent* ctrl = obj->add<ControllerComponent>();
            float speed;
            if (elem->QueryFloatAttribute("speed", &speed) == tinyxml2::XML_SUCCESS) {
                ctrl->setSpeed(speed);
            }
        };

        // ---------------- Behavior1Component ----------------
        map["Behavior1Component"] = [](GameObject* obj, const tinyxml2::XMLElement* elem) {
            Behavior1Component* behavior = obj->add<Behavior1Component>();
            elem->QueryFloatAttribute("speed", &behavior->speed);
            elem->QueryFloatAttribute("minX", &behavior->minX);
            elem->QueryFloatAttribute("maxX", &behavior->maxX);
        };
    }

    void createComponent(GameObject* obj, const tinyxml2::XMLElement* elem) {
        const char* name = elem->Name();
        if (map.count(name)) map[name](obj, elem);
    }
};
