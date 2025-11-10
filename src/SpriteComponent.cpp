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
}


void SpriteComponent::render() {
    // Placeholder implementation
    Object* object = getObject();
    BodyComponent* body = object->getComponent<BodyComponent>();
    if(!body) {
        std::cout << "BodyComponent not found" << std::endl;
        return;
    }
    // std::cout << "SPRITE ANGLE: " << body->getAngle() << std::endl;
    Engine::drawImage(textureName,body->getX(), body->getY(), body->getWidth(), body->getHeight(),body->getAngle());
}
