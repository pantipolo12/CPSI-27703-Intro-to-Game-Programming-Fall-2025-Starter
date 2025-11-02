#pragma once
#include <SDL.h>

class GameObject;
class Input;

class Component {
public:
    virtual ~Component() = default;

    virtual void update(float dt, const Input& input) = 0;
    virtual void draw(SDL_Renderer* renderer) = 0;

    void setParent(GameObject* p) { parentObj = p; }
    GameObject& parent() { return *parentObj; }

private:
    GameObject* parentObj = nullptr;
};
