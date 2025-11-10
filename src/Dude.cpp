#include "Dude.h"
#include "Engine.h"
#include "ImageDevice.h"
#include "InputDevice.h"
#include <SDL.h>
#include <iostream>

Dude::Dude(float x, float y)
    : Object(x, y, 100, 100) {
    std::cout << "Dude created at (" << x << ", " << y << ")"<< std::endl;
}



void Dude::update() {
    if(InputDevice::isKeyDown(SDLK_LEFT))
        setX(getX()-5);
    if(InputDevice::isKeyDown(SDLK_RIGHT))
        setX(getX()+5);
    if(InputDevice::isKeyDown(SDLK_UP))
        setY(getY()-5);
    if(InputDevice::isKeyDown(SDLK_DOWN))
        setY(getY()+5);
}

void Dude::render() {
    Engine::drawImage(getX(), getY(), getWidth(), getHeight(), "dude");
}


