#pragma once
#include <SDL.h>
#include <unordered_map>

class Input {
public:
    void update() {
        const Uint8* state = SDL_GetKeyboardState(nullptr);
        for (auto& key : keysDown) {
            key.second = state[key.first];
        }
    }

    bool isKeyDown(SDL_Scancode key) const {
        auto it = keysDown.find(key);
        return it != keysDown.end() ? it->second : false;
    }

    void registerKey(SDL_Scancode key) {
        keysDown[key] = false;
    }

private:
    std::unordered_map<SDL_Scancode, bool> keysDown;
};
