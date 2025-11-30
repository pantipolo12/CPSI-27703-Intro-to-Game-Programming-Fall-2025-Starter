#include "Engine.h"
#include "Object.h"
#include "ImageDevice.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "InputDevice.h"
#include "BodyComponent.h"
#include "CharacterComponent.h"

Engine* Engine::E = nullptr;

Engine::Engine() {

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    width = 800;
    height = 600;
    window = SDL_CreateWindow("Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_GetWindowSize(window, &width, &height);
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
    processInput();
    updateObjects();

    debugPlayerPosition(player);
    // Update camera
    updateView(player);
    render();
}

void Engine::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_WINDOW_RESIZABLE){
            width = event.window.data1;
            height = event.window.data2;
        }
        if (event.type == SDL_QUIT) {
            exit(0);
        }
        InputDevice::process(event);
    }
}

void Engine::updateObjects() {
    // Update all objects
    for (auto& obj : objects) 
        obj->update(dt); 
        
}

void Engine::render()
{
    if (!renderer) return;

    // Clear screen (black background)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render all objects
    for (auto& obj : objects) {
        obj->render();  // pass the camera view
        BodyComponent* body = obj->getComponent<BodyComponent>();
        // if (body) {
        //     drawRect(body->getX(), body->getY(), body->getWidth(), body->getHeight(), 255, 0, 0, 150);
        // }
    }
    

    // Optional: draw the ground line
    drawRect(0, groundY, view.worldWidth, 2, 0, 255, 0);

    // Present the final frame
    SDL_RenderPresent(renderer);
}

void Engine::setView(int x, int y) {
    view.x = x;
    view.y = y;
}
void Engine::updateView(Object* player) {
    if (!player) return;

    BodyComponent* body = player->getComponent<BodyComponent>();
    if (!body) return;

    float px = body->getX() + body->getWidth() / 2;
    float py = body->getY() + body->getHeight() / 2;

    // Use View's method to center camera
    view.centerOn(px, py);
}

void Engine::drawRect(float x, float y, float w, float h, int r, int g, int b, int a)
{
    if (!renderer) return;

    // Make an SDL_Rect in world space
    SDL_Rect rect;
    rect.x = int(x);
    rect.y = int(y);
    rect.w = int(w);
    rect.h = int(h);

    // Apply camera transform
    rect = view.transform(rect);

    // Draw the rectangle
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
}

void Engine::drawImage(std::string textureName, float x, float y, float w, float h, float angle, bool centerOrigin ) {
    SDL_Texture* tex = ImageDevice::get(textureName);
    SDL_Rect rect;
    SDL_Point* pCenter = nullptr;
    
    if (centerOrigin) {
        rect = {(int)(x - view.x - w / 2), (int)(y - view.y - h / 2), (int)w, (int)h};
        static SDL_Point centerPoint; 
        centerPoint = {(int)(w/2), (int)(h/2)};
        pCenter = &centerPoint;
    } else {
        rect = {(int)(x - view.x), (int)(y - view.y), (int)w, (int)h};
        pCenter = nullptr; // top-left origin
    }

    SDL_RenderCopyEx(renderer, tex, nullptr, &rect, angle, pCenter, SDL_FLIP_NONE);
}

void Engine::debugDrawObjects() {
    if (!renderer) return;

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // bright green

    for (auto& obj : objects) {
        BodyComponent* body = obj->getComponent<BodyComponent>();
        if (!body) continue;

        // World rectangle
        SDL_Rect worldRect = {
            int(body->getX()),
            int(body->getY()),
            int(body->getWidth()),
            int(body->getHeight())
        };

        // Transform with camera
        SDL_Rect screenRect = view.transform(worldRect);

        SDL_RenderDrawRect(renderer, &screenRect);
    }
}

void Engine::debugPlayerPosition(Object* player) {
    if (!player) return;
    BodyComponent* body = player->getComponent<BodyComponent>();
    if (!body) return;

    float px = body->getX();
    float py = body->getY();

    std::cout << "Player Position -> X: " << px
              << ", Y: " << py
              << " | Camera -> X: " << view.x
              << ", Y: " << view.y
              << std::endl;
}

Object* Engine::findObjectById(const std::string& id)
{
    for (auto& obj : objects)
        if (obj->getId() == id)
            return obj.get();  
    return nullptr;
}

void Engine::update(float dt) {
    for(auto& obj : getObjects()) {
        if (auto* charComp = obj->getComponent<CharacterComponent>()) {
            charComp->update(dt);
        }
        // other updates
    }
}
