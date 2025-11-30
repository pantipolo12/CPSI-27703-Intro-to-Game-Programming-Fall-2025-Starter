#include "BodyComponent.h"
#include "GroundComponent.h"
#include "SpriteComponent.h" // if needed later for rendering
#include "Engine.h"
#include "ImageDevice.h"
#include "LevelLoader.h"
#include <SDL.h>
#include <iostream>

int main(int argc, char* argv[])
{
    Engine e;

    // STEP 1: Load all textures (from XML)
    if (!ImageDevice::loadFromXML("assets/assets.xml")) {
        std::cerr << "Failed to load assets.xml" << std::endl;
        return -1;
    }

    // STEP 2: Load all game objects (from XML)
    if (!LevelLoader::load("assets/level.xml", e)) {
        std::cerr << "Failed to load level.xml" << std::endl;
        return -1;
    }

    // STEP 3: Configure the world
    e.setWorldSize(2000, 1200);

    // STEP 4: Main game loop with frame limiting
    const int targetFPS = 200;
    const int frameDelay = 1000 / targetFPS;
    Uint32 lastTime = SDL_GetTicks();

    while (true)
    {
        Uint32 frameStart = SDL_GetTicks();
        float dt = (frameStart - lastTime) / 1000.0f; // convert ms - secs
        lastTime = frameStart;

        e.update();

        SDL_Renderer* renderer = e.getRenderer();
        View& view = e.getView();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // clear screan

        for(auto& objPtr : e.getObjects())
        {
            Object* obj = objPtr.get();  // get raw pointer from unique_ptr
            if(auto* body = obj->getComponent<BodyComponent>())
            {
                SDL_Rect rect = view.transform(body->getRect());

                if(obj == e.getPlayer())
                {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // player = red
                } 
                else if(obj->getComponent<GroundComponent>())
                {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // ground = green
                }
                else 
                {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // other objects = blue
                }

                SDL_RenderDrawRect(renderer, &rect);
            }
        }

        SDL_RenderPresent(renderer);

        auto* playerBody = e.getPlayer()->getComponent<BodyComponent>(); // assuming player exists

        if(playerBody)
        {
            std::cout << "Player Y: " << playerBody->getY() << "\n";
        } 

        for(auto& obj : e.getObjects())
        {
            if(auto* ground = obj->getComponent<GroundComponent>())
            {
                BodyComponent* body = obj->getComponent<BodyComponent>();

                if(!body) continue;

                std::cout << "Ground Y: " << body->getY() << ", Height: " <<
                body->getHeight() << "\n";
            }
        }

        for(auto& obj : e.getObjects())
        {
            if(auto* body = obj->getComponent<BodyComponent>())
            {
                SDL_Rect rect = view.transform(body->getRect());
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red box
                SDL_RenderDrawRect(renderer, &rect);
            }
        }

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if(frameDelay > frameTime)
            SDL_Delay(frameDelay - frameTime);

        lastTime = SDL_GetTicks();

    }
    return 0;
}