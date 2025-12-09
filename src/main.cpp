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
            // Handle try again button click if game is over
            if (gameStarted && e.isGameOver() && event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                // Check if click is on "Try Again" button (center of screen)
                if (mouseX >= e.getWidth() / 2 - 75 && mouseX <= e.getWidth() / 2 + 75 &&
                    mouseY >= e.getHeight() / 2 && mouseY <= e.getHeight() / 2 + 40) {
                    e.resetGame();
                }
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
                // Normal game update (only if not game over)
                if (!e.isGameOver()) {
                    e.update();
                }

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
                        // For player, adjust collision box to match actual sprite size
                        SDL_Rect rect;
                        if(obj == e.getPlayer()) {
                            float px = body->getX();
                            float py = body->getY();
                            float pw = body->getWidth();
                            float ph = body->getHeight();
                            
                            // Reduce collision box size to match visible sprite (account for transparent padding)
                            // The sprite is 64x64 but has ~10px of transparent padding on each side
                            const float spritePadding = 10.0f;
                            float visibleWidth = pw - (spritePadding * 2.0f);
                            float visibleHeight = ph - (spritePadding * 2.0f);
                            
                            // Create rect with adjusted size to match visible sprite
                            SDL_Rect worldRect = {
                                int(px - visibleWidth / 2.0f),
                                int(py - visibleHeight / 2.0f),
                                int(visibleWidth),
                                int(visibleHeight)
                            };
                            rect = view.transform(worldRect);
                        } else {
                            rect = view.transform(body->getRect());
                        }

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
                
                // Render health UI and game over screen (handled in Engine::render)
                // But we need to call it here since main.cpp has its own render loop
                e.renderHealthUI();
                if (e.isGameOver()) {
                    e.renderGameOver();
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