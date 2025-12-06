#pragma once
#include "Component.h"
#include <SDL.h>
#include <string>

class AnimateComponent : public Component {
public:
    AnimateComponent(const std::string& textureName,
                     int frameCount,
                     float frameTime,
                     int frameWidth = 0,
                     int frameHeight = 0);

    void update(float dt) override;
    void render() override;

    void setFlip(SDL_RendererFlip flip);
    void setEnabled(bool enabled) { isEnabled = enabled; }
    bool getEnabled() const { return isEnabled; }

private:
    std::string textureName;
    SDL_Texture* texture = nullptr;

    int frameCount;
    int currentFrame = 0;

    float frameTime;      // time per frame (seconds)
    float frameTimer = 0; // countdown

    int frameWidth;
    int frameHeight;

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    bool isEnabled = true; // Only render when enabled
};
