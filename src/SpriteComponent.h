#pragma once
#include "Component.h"
#include <SDL2/SDL.h>
#include <string>
#include "SpriteComponent.h"

class SpriteComponent : public Component {
public:
    SpriteComponent(const std::string& textureName);
    SpriteComponent(int r, int g, int b); // Constructor for colored rectangles (RGB 0-255)

    // Getter
    const std::string& getTextureName() const { return textureName; }
    
    // Setter
    void setTextureName(const std::string& newTextureName) { textureName = newTextureName; }
    
    // Override render method with placeholder implementation
    void render() override;

    void draw();
    void draw(SDL_RendererFlip flip);
    
    void setParallax(float factor);
    
    // Position and size setters for background sprites
    void setX(float x) { spriteX = x; }
    void setY(float y) { spriteY = y; }
    void setWidth(float w) { width = w; }
    void setHeight(float h) { height = h; }
    
    // Position and size getters
    float getX() const { return spriteX; }
    float getY() const { return spriteY; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getParallax() const { return parallaxFactor; }
    
    // Enable/disable rendering (useful when AnimateComponent is active)
    void setEnabled(bool enabled) { isEnabled = enabled; }
    bool getEnabled() const { return isEnabled; }
    
    // Flip support
    void setFlip(SDL_RendererFlip flip) { this->flip = flip; }
    SDL_RendererFlip getFlip() const { return flip; }

    SDL_Texture* getTexture() const { return texture;}
private:
    bool screenSpace = false;   // does NOT use the camera transform
    bool isEnabled = true;      // Enable/disable rendering
    SDL_RendererFlip flip = SDL_FLIP_NONE; // Flip state
    std::string textureName;
    SDL_Texture* texture = nullptr;
    float spriteX = 0;
float spriteY = 0;
float width = 0;
float height = 0;
float parallaxFactor = 1.0f; // 1 = normal, <1 = slow background, >1 = fast foreground 
float lastParallaxOffsetX = 0.0f; // Track previous parallax offset for bidirectional scrolling

};
