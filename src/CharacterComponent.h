#pragma once
#include "Component.h"

class CharacterComponent : public Component {
public:
    
    void update(float dt) override;
    void drawDebug(SDL_Renderer* renderer);
};

