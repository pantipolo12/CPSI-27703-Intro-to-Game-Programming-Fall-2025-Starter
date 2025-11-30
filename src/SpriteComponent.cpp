#include "SpriteComponent.h"
#include "BodyComponent.h"
#include "ImageDevice.h"
#include "Engine.h"
#include <iostream>

SpriteComponent::SpriteComponent(const std::string& textureName) 
    : textureName(textureName) {
}

void SpriteComponent::draw() {
    BodyComponent* body = getObject()->getComponent<BodyComponent>();
    if (!body) return;

    SDL_Renderer* renderer = Engine::E->getRenderer();
    SDL_Texture* tex = ImageDevice::get(textureName);
    if (!tex) return;

    // Create world rectangle
    SDL_Rect worldRect = { 
        int(body->getX()), 
        int(body->getY()), 
        int(body->getWidth()), 
        int(body->getHeight()) 
    };

    // Transform world rect to screen rect using the view
    SDL_Rect screenRect = Engine::E->getView().transform(worldRect);

    SDL_RenderCopy(renderer, tex, nullptr, &screenRect);
    std::cout << "DRAW "
          << textureName 
          << " worldY=" << body->getY()
          << " screenY=" << screenRect.y
          << " camY=" << Engine::E->getView().y
          << std::endl;

}
void SpriteComponent::render() {
    draw();  // Move all drawing into one unified method
}

void SpriteComponent::setParallax(float factor)
{
    parallaxFactor = factor;
    screenSpace = true; // render ignoring camera
}