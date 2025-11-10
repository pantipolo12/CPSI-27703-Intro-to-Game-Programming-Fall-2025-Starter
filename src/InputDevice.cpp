#include "InputDevice.h"

std::unordered_set<SDL_Scancode> InputDevice::pressedKeys;

void InputDevice::process(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            if (!event.key.repeat) {
                pressedKeys.insert(event.key.keysym.scancode);
            }
            break;
        case SDL_KEYUP:
            pressedKeys.erase(event.key.keysym.scancode);
            break;
        default:
            break;
    }
}

bool InputDevice::isKeyDown(SDL_Scancode key) {
    return pressedKeys.find(key) != pressedKeys.end();
}
