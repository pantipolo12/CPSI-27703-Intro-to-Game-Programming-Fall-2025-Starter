#include "SpriteComponent.h"
#include "BodyComponent.h"
#include "ImageDevice.h"
#include "Engine.h"
#include <iostream>

SpriteComponent::SpriteComponent(const std::string& textureName) 
    : textureName(textureName) {
}

SpriteComponent::SpriteComponent(int r, int g, int b) {
    // Create a simple colored rectangle texture
    // We'll use a special naming convention for colored sprites
    textureName = "_COLOR_" + std::to_string(r) + "_" + std::to_string(g) + "_" + std::to_string(b);
    
    // Create a 1x1 colored texture
    SDL_Renderer* renderer = Engine::E ? Engine::E->getRenderer() : nullptr;
    if (renderer) {
        SDL_Surface* surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 0, 0);
        if (surface) {
            SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, r, g, b));
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }
    }
}

// ********** ADD THIS MISSING FUNCTION **********
void SpriteComponent::draw() {
    draw(SDL_FLIP_NONE);
}
// ***********************************************

void SpriteComponent::draw(SDL_RendererFlip flip) {
    SDL_Renderer* renderer = Engine::E->getRenderer();
    SDL_Texture* tex = nullptr;
    
    // Check if this is a colored rectangle (starts with "_COLOR_")
    if (textureName.find("_COLOR_") == 0) {
        tex = texture; // Use the texture we created
    } else {
        tex = ImageDevice::get(textureName);
    }
    
    if (!tex) return;

    SDL_Rect worldRect;

    BodyComponent* body = getObject()->getComponent<BodyComponent>();

    if (body) {
        // Object with physics uses the body
        worldRect = body->getRect();
    } else {
        // STATIC IMAGE (backgrounds, clouds, trees)
        worldRect.x = spriteX;
        worldRect.y = spriteY;
        worldRect.w = width;
        worldRect.h = height;
    }

    SDL_Rect screenRect;

    if (screenSpace) {
        // --- PARALLAX LOGIC ---
        // Parallax: 0.0 = no movement (sky), 1.0 = full movement (foreground)
        // Lower parallax values move slower (background layers)
        float camX = Engine::E->getView().x;
        float camY = Engine::E->getView().y;
        
        // Calculate parallax offset - background layers move slower than camera
        float parallaxOffsetX = camX * parallaxFactor;
        float parallaxOffsetY = camY * parallaxFactor;
        
        // Apply parallax to world position
        screenRect.x = int(worldRect.x - parallaxOffsetX);
        screenRect.y = int(worldRect.y - parallaxOffsetY);
        screenRect.w = worldRect.w;
        screenRect.h = worldRect.h;
    }
    else {
        // Normal behavior (camera transform)
        screenRect = Engine::E->getView().transform(worldRect);
    }

    SDL_RenderCopyEx(renderer, tex, nullptr, &screenRect, 0.0, nullptr, flip);
}

void SpriteComponent::render() {
    if (!isEnabled) return; // Don't render if disabled
    draw();
}

void SpriteComponent::setParallax(float factor)
{
    parallaxFactor = factor;
    screenSpace = true;
}
