#include "BodyComponent.h"
#include "GroundComponent.h"
#include "SpriteComponent.h" 
#include "Engine.h"
#include "ImageDevice.h"
#include "LevelLoader.h"
#include "Menu.h"
#include "InputDevice.h"
#include "SaveGame.h"
#include <SDL.h>
#include <iostream>

int main(int argc, char* argv[])
{
    Engine e;

    //  Load all textures
    if (!ImageDevice::loadFromXML("assets/assets.xml")) {
        std::cerr << "Failed to load assets.xml" << std::endl;
        return -1;
    }

    // Create menu
    Menu menu(e.getRenderer(), e.getWidth(), e.getHeight());
    bool gameStarted = false;
    bool shouldQuit = false;

    // Main loop with menu
    const int targetFPS = 60;
    const int frameDelay = 1000 / targetFPS;
    Uint32 lastTime = SDL_GetTicks();

    while (!shouldQuit)
    {
        Uint32 frameStart = SDL_GetTicks();
        float dt = (frameStart - lastTime) / 1000.0f;
        if (dt > 0.1f) dt = 0.1f;
        lastTime = frameStart;

        // Process input events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                shouldQuit = true;
            }
            InputDevice::process(event);
        }

        SDL_Renderer* renderer = e.getRenderer();

        if (!gameStarted) {
            // Show menu
            MenuAction action = menu.handleInput();
            
            switch (action) {
                case MenuAction::START_GAME:
                    // Load game
                    if (!LevelLoader::load("assets/level.xml", e)) {
                        std::cerr << "Failed to load level.xml" << std::endl;
                        return -1;
                    }
                    e.setWorldSize(5000, 1200);
                    gameStarted = true;
                    menu.setState(MenuState::IN_GAME);
                    break;
                case MenuAction::QUIT:
                    shouldQuit = true;
                    break;
                case MenuAction::OPTIONS:
                    // Options menu is handled in Menu::handleInput
                    break;
                default:
                    break;
            }

            menu.render();
        } else {
            // Game is running
            // Toggle pause menu with ESC
            const Uint8* keystate = SDL_GetKeyboardState(NULL);
            static bool escPressed = false;
            static bool pauseMenuActive = false;

            if (keystate[SDL_SCANCODE_ESCAPE] && !escPressed) {
                pauseMenuActive = !pauseMenuActive;
                if (pauseMenuActive) {
                    menu.setState(MenuState::PAUSE_MENU);
                    menu.setSelectedIndex(0);
                } else {
                    menu.setState(MenuState::IN_GAME);
                }
                escPressed = true;
            } else if (!keystate[SDL_SCANCODE_ESCAPE]) {
                escPressed = false;
            }

            if (pauseMenuActive) {
                // Show pause menu and handle input
                MenuAction action = menu.handleInput();

                switch (action) {
                    case MenuAction::SAVE_GAME: {
                        std::string savePath = SaveGame::getDefaultSavePath();
                        if (SaveGame::save(savePath, e)) {
                            std::cout << "Game saved!" << std::endl;
                        }
                        break;
                    }
                    case MenuAction::LOAD_GAME: {
                        std::string savePath = SaveGame::getDefaultSavePath();
                        if (SaveGame::exists(savePath)) {
                            if (SaveGame::load(savePath, e)) {
                                std::cout << "Game loaded!" << std::endl;
                                pauseMenuActive = false;
                                menu.setState(MenuState::IN_GAME);
                            }
                        } else {
                            std::cout << "No save file found!" << std::endl;
                        }
                        break;
                    }
                    case MenuAction::RESUME_GAME:
                        pauseMenuActive = false;
                        menu.setState(MenuState::IN_GAME);
                        break;
                    case MenuAction::QUIT:
                        shouldQuit = true;
                        break;
                    default:
                        break;
                }

                menu.render();
            } else {
                // Normal game update
                e.update();

                View& view = e.getView();

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);

                // render all sprites
                for(auto& objPtr : e.getObjects())
                {
                    Object* obj = objPtr.get();
                    obj->render();
                }

                //  draw debug boxes on top
                for(auto& objPtr : e.getObjects())
                {
                    Object* obj = objPtr.get();
                    if(auto* body = obj->getComponent<BodyComponent>())
                    {
                        SDL_Rect rect = view.transform(body->getRect());

                        if(obj == e.getPlayer())
                        {
                            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                        } 
                        else if(obj->getComponent<GroundComponent>())
                        {
                            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                        }
                        else 
                        {
                            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                        }

                        SDL_RenderDrawRect(renderer, &rect);
                    }
                }
            }
        }

        SDL_RenderPresent(renderer);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if(frameDelay > frameTime)
            SDL_Delay(frameDelay - frameTime);

        lastTime = SDL_GetTicks();
    }
    return 0;
}