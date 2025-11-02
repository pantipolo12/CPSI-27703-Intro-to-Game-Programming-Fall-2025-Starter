#pragma once
#include "Component.h"
#include <SDL.h>
#include <cstdio>
#include "Utils.h"
#include "BodyComponent.h"

class Input;

class SpriteComponent : public Component {
public:
    SDL_Texture* texture = nullptr;
    std::string texturePath; // store path to load later
    SDL_Color color = {255, 255, 255, 255};
    int width = 64, height = 64;

    void draw(SDL_Renderer* renderer) override {
        BodyComponent* body = parent().get<BodyComponent>();
        if (!body) return;
    
        SDL_FPoint pos = worldToScreen(body->x, body->y);
    
        SDL_Rect rect;
        rect.w = width;
        rect.h = height;
        rect.x = static_cast<int>(pos.x - width / 2);
        rect.y = static_cast<int>(pos.y - height);
    
        if (texture) {
            // Draw the loaded PNG texture
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
        } else {
            // Draw colored rectangle fallback
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &rect);
        }
    
        // Debug output (every 60 frames)
        static int count = 0;
        if (count++ % 60 == 0) {
            if(texture)
                printf("[Sprite] Drawing texture '%s' at world(%0.1f,%0.1f) screen(%d,%d) size(%d,%d)\n",
                       texturePath.c_str(), body->x, body->y, rect.x, rect.y, rect.w, rect.h);
            else
                printf("[Sprite] Drawing colored rect at world(%0.1f,%0.1f) screen(%d,%d) size(%d,%d) color(%d,%d,%d)\n",
                       body->x, body->y, rect.x, rect.y, rect.w, rect.h, color.r, color.g, color.b);
        }
    }
    
    void update(float dt, const Input& input) override {}

};