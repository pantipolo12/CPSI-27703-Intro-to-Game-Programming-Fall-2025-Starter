#pragma once
#include "Component.h"
#include <SDL.h>

class CharacterComponent : public Component {
public:
    
    void update(float dt) override;
    void drawDebug(SDL_Renderer* renderer);
    
private:
    SDL_RendererFlip lastFlip = SDL_FLIP_NONE; // Remember last flip direction
};

