#pragma once
#include "Component.h"
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
private:
    std::string textureName;
};
