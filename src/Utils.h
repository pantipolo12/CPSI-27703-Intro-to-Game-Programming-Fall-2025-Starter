#pragma once
#include <SDL.h>

const float WORLD_SCALE = 100.0f;
const int SCREEN_HEIGHT = 600;  // match your SDL window height

// Converts world (x, y) to screen (pixel) coordinates
inline SDL_FPoint worldToScreen(float worldX, float worldY) {
    SDL_FPoint screenPos;
    screenPos.x = worldX * WORLD_SCALE;
    screenPos.y = SCREEN_HEIGHT - (worldY * WORLD_SCALE);  // y = 0 is bottom of screen
    return screenPos;
}