#include <SDL.h>
#include <SDL_image.h>
#include "Engine.h"
#include "Input.h"

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        SDL_Log("Failed to init SDL_image: %s", IMG_GetError());
        return 1;
    }
    

    SDL_Window* window = SDL_CreateWindow("Component Game", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    
    ComponentFactory factory(renderer);


    // Create the engine
    Engine engine("assets/level.xml", renderer);

    // Create input handler
    Input input;

    // Register keys for movement
    input.registerKey(SDL_SCANCODE_W);
    input.registerKey(SDL_SCANCODE_A);
    input.registerKey(SDL_SCANCODE_S);
    input.registerKey(SDL_SCANCODE_D);

    bool running = true;
    Uint32 lastTime = SDL_GetTicks();

    while (running) {
        // Calculate delta time (dt)
        Uint32 currentTime = SDL_GetTicks();
        float dt = (currentTime - lastTime) / 1000.0f; // convert ms to seconds
        lastTime = currentTime;

        // Handle SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        // Update input state
        input.update();

        // Update engine with input and delta time
        engine.update(input, dt);

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        // Draw all game objects
        engine.draw(renderer);

        // Present the frame
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}