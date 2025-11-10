#include "Engine.h"
#include "Object.h"
#include "ImageDevice.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "InputDevice.h"
#include "BodyComponent.h"

Engine* Engine::E = nullptr;

Engine::Engine() {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    width = 800;
    height = 600;
    window = SDL_CreateWindow("Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    E = this;
}
Engine::~Engine() {
    //ImageDevice::cleanup();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    IMG_Quit();
    SDL_Quit();
}

Object* Engine::addObject() {
    objects.push_back(std::make_unique<Object>());
    return objects.back().get();
}


void Engine::update() {
    // view.x+=1;
    // view.y+=1;
    processInput();
    updateObjects();

    // Make camera follow player
    Object* player = getLastObject(); // or get your specific player object
    //followPlayer(player);
    render();
}

void Engine::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            exit(0);
        }
        InputDevice::process(event);
    }
}

void Engine::updateObjects() {
    // Update all objects
    for (auto& obj : objects) 
        obj->update();
        
}

void Engine::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    for (auto& obj : objects) 
        obj->render();
    SDL_RenderPresent(renderer);

}

// void Engine::drawRect(float x, float y, float width, float height, int r, int g, int b, int a) {
//     SDL_SetRenderDrawColor(Engine::E->renderer, r, g, b, a);
//     SDL_Rect rect = {(int)x, (int)y, (int)width, (int)height};
//     SDL_RenderFillRect(Engine::E->renderer, &rect);
// }


void Engine::drawRect(float x, float y, float width, float height, int r, int g, int b, int a) {
    
    View& view = Engine::E->view;
    SDL_SetRenderDrawColor(Engine::E->renderer, r, g, b, a);
    SDL_Rect rect = {(int)(x-view.x), (int)(y-view.y), (int)width, (int)height};
    SDL_RenderFillRect(Engine::E->renderer, &rect);
}

void Engine::drawImage( std::string textureName, float x, float y, float width, float height, float angle) {
    
    View& view = Engine::E->view;
    SDL_Texture* imgTexture;

    imgTexture = ImageDevice::get(textureName);
    SDL_Rect rect = {(int)(x-view.x-width/2), (int)(y-view.y-height/2), (int)width, (int)height};
    SDL_Point center = {(int)(width/2), (int)(height/2)};
    SDL_RenderCopyEx(Engine::E->renderer, imgTexture, nullptr, &rect, angle,&center, SDL_FLIP_NONE);
}

void Engine::setView(int x, int y) {
    view.x = x;
    view.y = y;
}


// void Engine::followPlayer(Object* player) {
//     BodyComponent* body = player->getComponent<BodyComponent>();
//     if (!body) return;

//     // For side-scrolling, we often only move horizontally
//     view.x = body->getX() + body->getWidth() / 2;  // center view on player horizontally
//     view.y = view.y; // keep vertical static if you want horizontal scrolling only

//     // Optional: clamp view to world bounds so camera doesn’t go outside the level
//     // view.x = std::max(view.x, levelMinX);
//     // view.x = std::min(view.x, levelMaxX);
// }

void Engine::updateView(Object* player) {
    BodyComponent* body = player->getComponent<BodyComponent>();
    if (!body) return;

    const float playerCenterX = body->getX() + body->getWidth() / 2;
    const float screenCenterX = width / 2;

    // Start moving camera when player reaches middle of the screen
    if (playerCenterX > screenCenterX) {
        view.x = playerCenterX - screenCenterX;
    }

    // Optional vertical follow
    // view.y = body->getY() + body->getHeight() / 2 - height / 2;
}

