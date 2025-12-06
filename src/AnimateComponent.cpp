#include "AnimateComponent.h"
#include "Engine.h"
#include "BodyComponent.h"
#include "ImageDevice.h"

AnimateComponent::AnimateComponent(const std::string& textureName,
                                   int frameCount,
                                   float frameTime,
                                   int frameWidth,
                                   int frameHeight)
    : textureName(textureName),
      frameCount(frameCount),
      frameTime(frameTime),
      frameWidth(frameWidth),
      frameHeight(frameHeight)
{
    texture = ImageDevice::get(textureName);

    // If no width/height provided, auto-detect from texture
    if (frameWidth == 0 || frameHeight == 0) {
        int texW, texH;
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        this->frameWidth  = texW / frameCount;
        this->frameHeight = texH;
    }
}

void AnimateComponent::update(float dt) {
    frameTimer += dt;

    if (frameTimer >= frameTime) {
        frameTimer = 0;
        currentFrame = (currentFrame + 1) % frameCount;
    }
}

void AnimateComponent::render() {
    if (!isEnabled) return; // Don't render if disabled
    
    SDL_Renderer* renderer = Engine::E->getRenderer();
    if (!renderer || !texture) return;

    BodyComponent* body = getObject()->getComponent<BodyComponent>();
    if (!body) return;

    // Source (which frame to draw)
    SDL_Rect src {
        currentFrame * frameWidth,
        0,
        frameWidth,
        frameHeight
    };

    // Destination (object's location)
    SDL_Rect dest = Engine::E->getView().transform(body->getRect());

    SDL_RenderCopyEx(renderer, texture, &src, &dest, 0, NULL, flip);
}

void AnimateComponent::setFlip(SDL_RendererFlip f) {
    flip = f;
}
