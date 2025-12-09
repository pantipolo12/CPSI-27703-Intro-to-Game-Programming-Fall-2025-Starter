#include "AnimateComponent.h"
#include "Engine.h"
#include "BodyComponent.h"
#include "ImageDevice.h"
#include <iostream>

AnimateComponent::AnimateComponent(const std::string& textureName,
                                   int frameCount,
                                   float frameTime,
                                   int frameWidth,
                                   int frameHeight,
                                   int frameSpacing)
    : textureName(textureName),
      frameCount(frameCount),
      frameWidth(frameWidth),
      frameHeight(frameHeight),
      frameSpacing(frameSpacing),
      frameTimer(frameTime, true)  // Initialize timer with frameTime and start running
{
    texture = ImageDevice::get(textureName);

    // If no width/height provided, auto-detect from texture
    if ((frameWidth == 0 || frameHeight == 0) && texture != nullptr) {
        int texW, texH;
        if (SDL_QueryTexture(texture, NULL, NULL, &texW, &texH) == 0) {
            // Account for spacing: totalWidth = (frameWidth * frameCount) + (spacing * (frameCount - 1))
            // So: frameWidth = (texW - spacing * (frameCount - 1)) / frameCount
            if (frameSpacing > 0) {
                this->frameWidth = (texW - frameSpacing * (frameCount - 1)) / frameCount;
            } else {
                this->frameWidth = texW / frameCount;
            }
            this->frameHeight = texH;
            std::cout << "AnimateComponent: Auto-detected frame size for '" << textureName 
                      << "': " << this->frameWidth << "x" << this->frameHeight 
                      << " (total: " << texW << "x" << texH << ", frames: " << frameCount 
                      << ", spacing: " << frameSpacing << ")\n";
        } else {
            std::cerr << "AnimateComponent: Failed to query texture dimensions for '" << textureName << "'\n";
        }
    } else if (texture == nullptr) {
        std::cerr << "AnimateComponent: Texture '" << textureName << "' not found!\n";
    }
}

void AnimateComponent::update(float dt) {
    if (!isEnabled) {
        // Don't update if disabled
        return;
    }
    if (!texture) return; // Can't animate without texture
    
    // If frameCount is 1 or 0, this is a single-frame image, don't animate
    if (frameCount <= 1) return;
    
    // Ensure timer is running
    if (!frameTimer.running()) {
        frameTimer.start();
    }
    
    // Update timer and check if it has expired
    if (frameTimer.update(dt)) {
        // Timer expired - advance to next frame
        int oldFrame = currentFrame;
        currentFrame = (currentFrame + 1) % frameCount;
        
        // Reset timer for next frame
        frameTimer.reset();
        
        // Debug: print every frame change to see what's happening
        std::cout << "AnimateComponent '" << textureName << "': frame " << oldFrame 
                  << " -> " << currentFrame << " (frames: " << frameCount 
                  << ", enabled: " << isEnabled << ", dt: " << dt << ")\n";
    }
}

void AnimateComponent::render() {
    if (!isEnabled) return; // Don't render if disabled
    
    SDL_Renderer* renderer = Engine::E->getRenderer();
    if (!renderer || !texture) {
        if (isEnabled) {
            std::cerr << "AnimateComponent: Cannot render '" << textureName 
                      << "' - renderer: " << (renderer ? "OK" : "NULL") 
                      << ", texture: " << (texture ? "OK" : "NULL") << "\n";
        }
        return;
    }

    BodyComponent* body = getObject()->getComponent<BodyComponent>();
    if (!body) {
        if (isEnabled) {
            std::cerr << "AnimateComponent: No BodyComponent for '" << textureName << "'\n";
        }
        return;
    }

    // Validate frame dimensions
    if (frameWidth <= 0 || frameHeight <= 0) {
        std::cerr << "AnimateComponent: Invalid frame dimensions for '" << textureName 
                  << "': " << frameWidth << "x" << frameHeight << "\n";
        return;
    }

    // Source (which frame to draw)
    SDL_Rect src;
    if (frameCount > 1) {
        // Sprite sheet - account for spacing between frames
        // Frame X position = currentFrame * (frameWidth + spacing)
        int srcX = currentFrame * (frameWidth + frameSpacing);
        src = {
            srcX,
            0,
            frameWidth,
            frameHeight
        };
        
        // Debug: print occasionally to verify frame calculation
        static int renderDebugCounter = 0;
        if (++renderDebugCounter % 180 == 0) {
            int texW, texH;
            SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
            std::cout << "AnimateComponent render: texture='" << textureName 
                      << "', currentFrame=" << currentFrame 
                      << ", srcX=" << srcX 
                      << ", frameWidth=" << frameWidth 
                      << ", spacing=" << frameSpacing
                      << ", textureSize=" << texW << "x" << texH << "\n";
        }
    } else {
        // Single frame - use full texture
        int texW, texH;
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        src = { 0, 0, texW, texH };
    }

    // Destination (object's location)
    SDL_Rect dest = Engine::E->getView().transform(body->getRect());

    SDL_RenderCopyEx(renderer, texture, &src, &dest, 0, NULL, flip);
}

void AnimateComponent::setFlip(SDL_RendererFlip f) {
    flip = f;
}

void AnimateComponent::setFrameTime(float newFrameTime) {
    frameTimer.setDuration(newFrameTime);
    frameTimer.reset();
    frameTimer.start(); // Ensure timer is running
}

void AnimateComponent::setTexture(const std::string& newTextureName) {
    // Use current frame count and dimensions
    setTexture(newTextureName, frameCount, frameWidth, frameHeight, frameSpacing);
}

void AnimateComponent::setTexture(const std::string& newTextureName, int newFrameCount) {
    // Use current frame dimensions and spacing
    setTexture(newTextureName, newFrameCount, frameWidth, frameHeight, frameSpacing);
}

void AnimateComponent::setTexture(const std::string& newTextureName, int newFrameCount, int newFrameWidth, int newFrameHeight, int newFrameSpacing) {
    if (textureName == newTextureName && texture != nullptr && frameCount == newFrameCount 
        && frameWidth == newFrameWidth && frameHeight == newFrameHeight && frameSpacing == newFrameSpacing) {
        return; // Already using this texture with same settings
    }
    
    textureName = newTextureName;
    frameCount = newFrameCount;
    frameWidth = newFrameWidth;
    frameHeight = newFrameHeight;
    frameSpacing = newFrameSpacing;
    
    SDL_Texture* newTexture = ImageDevice::get(textureName);
    
    if (!newTexture) {
        std::cerr << "AnimateComponent: Failed to load texture '" << textureName << "'\n";
        texture = nullptr;
        return;
    }
    
    texture = newTexture;
    
    // Verify dimensions match
    int texW, texH;
    if (SDL_QueryTexture(texture, NULL, NULL, &texW, &texH) == 0) {
        std::cout << "AnimateComponent: Switched to '" << textureName 
                  << "': frame size " << frameWidth << "x" << frameHeight 
                  << " (total: " << texW << "x" << texH << ", frames: " << frameCount 
                  << ", spacing: " << frameSpacing << ")\n";
    }
    
    // Reset animation
    currentFrame = 0;
    frameTimer.reset();
    frameTimer.start(); // Ensure timer is running after texture switch
    std::cout << "AnimateComponent: Switched texture to '" << textureName 
              << "', reset frame to 0, timer started\n";
}
