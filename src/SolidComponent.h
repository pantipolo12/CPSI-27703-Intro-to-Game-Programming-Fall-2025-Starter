#pragma once
#include "Component.h"
#include <SDL2/SDL.h>
#include <string>

class SpriteComponent : public Component {
public:
    SpriteComponent(const std::string& textureName);
    
    // Getter
    const std::string& getTextureName() const { return textureName; }
    
    // Setter
    void setTextureName(const std::string& newTextureName) { textureName = newTextureName; }
    
    // Override render method with placeholder implementation
    void render() override;

    void draw();
    
    void setParallax(float factor);

    SDL_Texture* getTexture() const { return texture;}
private:
    bool screenSpace = false;   // does NOT use the camera transform
    float parallaxFactor = 1.0f;
    std::string textureName;
    SDL_Texture* texture = nullptr;
};
