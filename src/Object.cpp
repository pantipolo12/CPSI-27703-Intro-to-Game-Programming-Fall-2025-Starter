#include "Object.h"
#include "Engine.h"
#include <SDL.h>
#include <iostream>

// Template implementations are now in Object.h

void Object::update() {
    for ( auto & [ name, component ] : components) {
        component->update();
    }
}

void Object::render() {
    for ( auto & [ name, component ] : components) {
        component->render();
    }
}