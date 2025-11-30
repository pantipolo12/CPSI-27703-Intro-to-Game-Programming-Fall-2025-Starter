#include "BackgroundComponent.h"
#include "BodyComponent.h"
#include "Object.h"
#include "SpriteComponent.h"
#include "SDL.h"
#include "Engine.h"
#include "View.h"
#include <iostream>

BackgroundComponent::update() {
    BodyComponent* body = getObject()->getComponent<BackgroundComponent>();

    if(!body) return;
}