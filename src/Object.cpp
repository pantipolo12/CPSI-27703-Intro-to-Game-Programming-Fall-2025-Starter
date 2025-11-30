#include "Object.h"
#include "Engine.h"
#include <SDL.h>
#include "View.h"
#include <iostream>


// Template implementations are now in Object.h


void Object::update(float dt) {
    for ( auto & [ name, component ] : components) {
        component->update(dt);
    }
}


void Object::render() {
    for ( auto & [ name, component ] : components) {
        component->render();
    }
}
