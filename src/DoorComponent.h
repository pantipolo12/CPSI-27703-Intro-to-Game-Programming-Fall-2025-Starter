#pragma once
#include "Component.h"

class DoorComponent : public Component {
public:
    DoorComponent() = default;
    void update(float dt) override;
    void setNextLevel(const std::string& level) { nextLevel = level; }
    
private:
    bool isOpen = false;
    std::string nextLevel = "assets/level2.xml"; // Default next level
};

