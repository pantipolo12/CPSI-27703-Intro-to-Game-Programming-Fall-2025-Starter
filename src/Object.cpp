#include "Object.h"
#include "Engine.h"
#include "BodyComponent.h"
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

void Object::initializeBodyComponentUserData() {
    // Find BodyComponent and initialize its userData
    for (auto& [name, component] : components) {
        if (auto* bodyComp = dynamic_cast<BodyComponent*>(component.get())) {
            bodyComp->initializeUserData();
        }
    }
}
