//#include "Game.h"
#include <SDL.h>
#include <iostream>
#include "Engine.h"

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const float WORLD_SCALE = 100.0f;

int groundY = SCREEN_HEIGHT - (int)(1.0f * WORLD_SCALE);  // World Y = 1


int main(int argc, char* argv[]) {

        SDL_Init(SDL_INIT_VIDEO);
        SDL_Window* window = SDL_CreateWindow("GAME", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
        // Load the level
        Engine engine("assets/level.xml");
    
        bool running = true;
        int mouseX = 0, mouseY = 0;
    
        while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT)
                    running = false;
                if (event.type == SDL_MOUSEMOTION) {
                    mouseX = event.motion.x;
                    mouseY = event.motion.y;
                }
            }
    
            // Clear screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
    
            // Update and draw objects
            engine.update();
            engine.draw(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderDrawLine(renderer, 0, groundY, SCREEN_WIDTH, groundY);

    
            // Present frame
            SDL_RenderPresent(renderer);
        }
    
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }


// Practice code

    // // Game g;
    // // return g.run();
    // SDL_Init(SDL_INIT_VIDEO);
    // SDL_Window* window = SDL_CreateWindow("GAME", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    // SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    // // SDL_RenderClear(renderer);
    // // SDL_RenderPresent(renderer);
    // bool running = true;
    // int x = 0, y = 0;
    // while (running) {
    //     SDL_Event event;
    //     while (SDL_PollEvent(&event)) {
    //         if (event.type == SDL_QUIT) {
    //             running = false;
    //         }
    //         if(event.type == SDL_MOUSEMOTION) {
    //             x = event.motion.x;
    //             y = event.motion.y;
    //         }
    //     }
    //     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //     SDL_RenderClear(renderer);
        
    //     SDL_Rect rect = {x-50, y-50, 100, 100};

    //     SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    //     SDL_RenderFillRect(renderer, &rect);


    //     SDL_RenderPresent(renderer);
    // }   
    // // SDL_DestroyRenderer(renderer);
    // SDL_DestroyWindow(window);
    // SDL_Quit(); 