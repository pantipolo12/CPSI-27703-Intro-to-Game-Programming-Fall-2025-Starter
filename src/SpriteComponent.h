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
    
    // Enable/disable rendering (useful when AnimateComponent is active)
    void setEnabled(bool enabled) { isEnabled = enabled; }
    bool getEnabled() const { return isEnabled; }

    SDL_Texture* getTexture() const { return texture;}
private:
    bool screenSpace = false;   // does NOT use the camera transform
    bool isEnabled = true;      // Enable/disable rendering
    std::string textureName;
    SDL_Texture* texture = nullptr;
    float spriteX = 0;
float spriteY = 0;
float width = 0;
float height = 0;
float parallaxFactor = 1.0f; // 1 = normal, <1 = slow background, >1 = fast foreground 

};
