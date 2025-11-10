#pragma once
#include <SDL.h>

class View {
public:
    float x = 0; // camera center x
    float y = 0; // camera center y
    float scale = 1.0f;

    // Transform world coordinates to screen coordinates
    SDL_Rect transform(const SDL_Rect& worldRect) const {
        SDL_Rect screen;
        screen.w = int(worldRect.w * scale);
        screen.h = int(worldRect.h * scale);
        screen.x = int((worldRect.x - x) * scale + 400); // 400 = screen center x
        screen.y = int((worldRect.y - y) * scale + 300); // 300 = screen center y
        return screen;
    }
};
