#pragma once
#include "Component.h"
#include <SDL.h>
#include <string>
#include "Timer.h"

class AnimateComponent : public Component {
public:
    AnimateComponent(const std::string& textureName,
                     int frameCount,
                     float frameTime,
                     int frameWidth = 0,
                     int frameHeight = 0,
                     int frameSpacing = 0);

    void update(float dt) override;
    void render() override;

    void setFlip(SDL_RendererFlip flip);
    void setEnabled(bool enabled) { isEnabled = enabled; }
    bool getEnabled() const { return isEnabled; }
    
    // Switch to a different texture/animation
    void setTexture(const std::string& newTextureName);
    void setTexture(const std::string& newTextureName, int newFrameCount);
    void setTexture(const std::string& newTextureName, int newFrameCount, int newFrameWidth, int newFrameHeight, int newFrameSpacing);
    
    // Set frame time (time per frame in seconds)
    void setFrameTime(float newFrameTime);
    
    // Get current texture name
    const std::string& getTextureName() const { return textureName; }

private:
    std::string textureName;
    SDL_Texture* texture = nullptr;

    int frameCount;
    int currentFrame = 0;

    Timer frameTimer;    // Timer for frame transitions

    int frameWidth;
    int frameHeight;
    int frameSpacing; // Pixels between frames (for sprite sheets with gaps)

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    bool isEnabled = true; // Start enabled by default
};
