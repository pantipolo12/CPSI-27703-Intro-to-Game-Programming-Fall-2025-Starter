#include "Menu.h"
#include <iostream>

Menu::Menu(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight),
      currentState(MenuState::MAIN_MENU), selectedIndex(0), font(nullptr), titleFont(nullptr)
{
    // Initialize SDL_ttf if not already done
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        }
    }

    // Try to load fonts (using default system fonts or fallback)
    // On Windows, try common font paths
    const char* fontPaths[] = {
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/times.ttf",
        "C:/Windows/Fonts/cour.ttf",
        nullptr
    };

    // Try to load title font
    for (int i = 0; fontPaths[i] != nullptr && !titleFont; ++i) {
        titleFont = TTF_OpenFont(fontPaths[i], 72);
    }
    if (!titleFont) {
        std::cerr << "Warning: Could not load title font. Menu will use rectangles instead of text." << std::endl;
    }

    // Try to load menu font
    for (int i = 0; fontPaths[i] != nullptr && !font; ++i) {
        font = TTF_OpenFont(fontPaths[i], 36);
    }
    if (!font) {
        std::cerr << "Warning: Could not load menu font. Menu will use rectangles instead of text." << std::endl;
    }

    // Menu items
    menuItems = {"Start Game", "Options", "Quit"};
}

Menu::~Menu() {
    if (font) {
        TTF_CloseFont(font);
    }
    if (titleFont) {
        TTF_CloseFont(titleFont);
    }
}

void Menu::render() {
    if (currentState == MenuState::MAIN_MENU) {
        // Clear screen with dark background
        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
        SDL_RenderClear(renderer);

        // Draw title
        SDL_Color titleColor = {255, 255, 100, 255}; // Yellow
        if (titleFont) {
            renderText("GIGI GAME", screenWidth / 2, 150, titleFont, titleColor);
        } else {
            // Fallback: draw title as rectangle
            SDL_Rect titleRect = {screenWidth / 2 - 200, 100, 400, 80};
            SDL_SetRenderDrawColor(renderer, titleColor.r, titleColor.g, titleColor.b, titleColor.a);
            SDL_RenderFillRect(renderer, &titleRect);
        }

        // Draw menu items
        int startY = screenHeight / 2;
        int spacing = 80;

        for (size_t i = 0; i < menuItems.size(); ++i) {
            SDL_Color color;
            if (i == selectedIndex) {
                color = {255, 255, 0, 255}; // Yellow for selected
            } else {
                color = {200, 200, 200, 255}; // Gray for unselected
            }

            if (font) {
                renderText(menuItems[i], screenWidth / 2, startY + (int)i * spacing, font, color);
            } else {
                // Fallback: draw rectangles if font not available
                SDL_Rect rect = {screenWidth / 2 - 100, startY + (int)i * spacing - 20, 200, 40};
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_RenderFillRect(renderer, &rect);
                // Draw a small indicator for selected item
                if (i == selectedIndex) {
                    SDL_Rect indicator = {screenWidth / 2 - 120, startY + (int)i * spacing - 10, 20, 20};
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                    SDL_RenderFillRect(renderer, &indicator);
                }
            }
        }

        // Draw instructions
        SDL_Color instructionColor = {150, 150, 150, 255};
        if (font) {
            renderText("Use Arrow Keys to navigate, Enter to select", 
                      screenWidth / 2, screenHeight - 100, font, instructionColor);
        }
    } else if (currentState == MenuState::OPTIONS) {
        // Options menu
        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
        SDL_RenderClear(renderer);

        SDL_Color titleColor = {255, 255, 100, 255};
        if (titleFont) {
            renderText("OPTIONS", screenWidth / 2, 150, titleFont, titleColor);
        }

        SDL_Color textColor = {200, 200, 200, 255};
        if (font) {
            renderText("Options menu - Press ESC to go back", 
                      screenWidth / 2, screenHeight / 2, font, textColor);
        }
    } else if (currentState == MenuState::PAUSE_MENU) {
        // Pause menu 
        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
        SDL_RenderClear(renderer);

        // Draw title
        SDL_Color titleColor = {255, 255, 100, 255}; // Yellow
        if (titleFont) {
            renderText("PAUSED", screenWidth / 2, 150, titleFont, titleColor);
        } else {
            // Fallback: draw title as rectangle
            SDL_Rect titleRect = {screenWidth / 2 - 200, 100, 400, 80};
            SDL_SetRenderDrawColor(renderer, titleColor.r, titleColor.g, titleColor.b, titleColor.a);
            SDL_RenderFillRect(renderer, &titleRect);
        }

        // Draw pause menu items
        std::vector<std::string> pauseItems = {"Save", "Load", "Resume", "Quit"};
        int startY = screenHeight / 2;
        int spacing = 80;

        for (size_t i = 0; i < pauseItems.size(); ++i) {
            SDL_Color color;
            if (i == selectedIndex) {
                color = {255, 255, 0, 255}; // Yellow for selected
            } else {
                color = {200, 200, 200, 255}; // Gray for unselected
            }

            if (font) {
                renderText(pauseItems[i], screenWidth / 2, startY + (int)i * spacing, font, color);
            } else {
                // Fallback: draw rectangles if font not available
                SDL_Rect rect = {screenWidth / 2 - 100, startY + (int)i * spacing - 20, 200, 40};
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_RenderFillRect(renderer, &rect);
                // Draw a small indicator for selected item
                if (i == selectedIndex) {
                    SDL_Rect indicator = {screenWidth / 2 - 120, startY + (int)i * spacing - 10, 20, 20};
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                    SDL_RenderFillRect(renderer, &indicator);
                }
            }
        }

        // Draw instructions
        SDL_Color instructionColor = {150, 150, 150, 255};
        if (font) {
            renderText("Use Arrow Keys to navigate, Enter to select", 
                      screenWidth / 2, screenHeight - 100, font, instructionColor);
        }
    }
}

MenuAction Menu::handleInput() {
    // Check for key presses
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    // Handle menu navigation
    static Uint32 lastKeyTime = 0;
    Uint32 currentTime = SDL_GetTicks();
    
    if (currentState == MenuState::MAIN_MENU) {
        // Arrow key navigation
        if ((keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W]) && (currentTime - lastKeyTime > 150)) {
            selectedIndex = (selectedIndex - 1 + menuItems.size()) % menuItems.size();
            lastKeyTime = currentTime;
        }
        if ((keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S]) && (currentTime - lastKeyTime > 150)) {
            selectedIndex = (selectedIndex + 1) % menuItems.size();
            lastKeyTime = currentTime;
        }

        // Enter key to select
        if (keystate[SDL_SCANCODE_RETURN] || keystate[SDL_SCANCODE_SPACE]) {
            switch (selectedIndex) {
                case 0: // Start Game
                    return MenuAction::START_GAME;
                case 1: // Options
                    currentState = MenuState::OPTIONS;
                    return MenuAction::OPTIONS;
                case 2: // Quit
                    return MenuAction::QUIT;
            }
        }
    } else if (currentState == MenuState::OPTIONS) {
        // ESC to go back
        if (keystate[SDL_SCANCODE_ESCAPE]) {
            currentState = MenuState::MAIN_MENU;
            selectedIndex = 0; // Reset selection
        }
    } else if (currentState == MenuState::PAUSE_MENU) {
        // Arrow key navigation for pause menu
        std::vector<std::string> pauseItems = {"Save", "Load", "Resume", "Quit"};
        if ((keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W]) && (currentTime - lastKeyTime > 150)) {
            selectedIndex = (selectedIndex - 1 + pauseItems.size()) % pauseItems.size();
            lastKeyTime = currentTime;
        }
        if ((keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S]) && (currentTime - lastKeyTime > 150)) {
            selectedIndex = (selectedIndex + 1) % pauseItems.size();
            lastKeyTime = currentTime;
        }

        // Enter key to select
        if (keystate[SDL_SCANCODE_RETURN] || keystate[SDL_SCANCODE_SPACE]) {
            switch (selectedIndex) {
                case 0: // Save
                    return MenuAction::SAVE_GAME;
                case 1: // Load
                    return MenuAction::LOAD_GAME;
                case 2: // Resume
                    return MenuAction::RESUME_GAME;
                case 3: // Quit
                    return MenuAction::QUIT;
            }
        }
    }

    return MenuAction::NONE;
}

void Menu::renderText(const std::string& text, int x, int y, TTF_Font* font, SDL_Color color) {
    SDL_Texture* texture = createTextTexture(text, font, color);
    if (!texture) return;

    int texW, texH;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);

    SDL_Rect dstRect = {x - texW / 2, y - texH / 2, texW, texH};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_DestroyTexture(texture);
}

SDL_Texture* Menu::createTextTexture(const std::string& text, TTF_Font* font, SDL_Color color) {
    if (!font) return nullptr;

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "TTF_RenderText_Solid failed: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}

