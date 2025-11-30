#pragma once
#include <SDL.h>

class View {
public:
    float x = 0; // top-left x of the camera
    float y = 0; // top-left y of the camera
    float scale = 1.0f;

    int screenWidth = 800;
    int screenHeight = 600;
    // World bounds for camera clamping
    int worldWidth = 10000;   // total width of your level
    int worldHeight = 600;  // total height of your level

    View() = default;
    View(int screenW, int screenH) : screenWidth(screenW), screenHeight(screenH) {}

    // Transform world coordinates to screen coordinates
    SDL_Rect transform(const SDL_Rect& worldRect) const {
        SDL_Rect screen;
        screen.w = int(worldRect.w * scale);
        screen.h = int(worldRect.h * scale);
        screen.x = int((worldRect.x - x) * scale);
        screen.y = int((worldRect.y - y) * scale);
        return screen;
    }

    // Center camera on a point (px, py), with optional clamping
    void centerOn(float px, float py) {
        x = px - screenWidth / 2.0f;
        y = py - screenHeight / 2.0f;

        // Clamp to world bounds
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > worldWidth - screenWidth) x = worldWidth - screenWidth;
        if (y > worldHeight - screenHeight) y = worldHeight - screenHeight;
    }

    void updateParallax(float playerX, float playerY, float factor)
    {
        x = playerX * factor - screenWidth * 0.5f;
        y = playerY * factor - screenHeight * 0.5f;

        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > worldWidth  - screenWidth)  x = worldWidth - screenWidth;
        if (y > worldHeight - screenHeight) y = worldHeight - screenHeight;
    }

};
