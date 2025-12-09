#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

enum class MenuState {
    MAIN_MENU,
    OPTIONS,
    IN_GAME,
    PAUSE_MENU,
    QUIT
};

enum class MenuAction {
    NONE,
    START_GAME,
    OPTIONS,
    QUIT,
    SAVE_GAME,
    LOAD_GAME,
    RESUME_GAME
};

class Menu {
public:
    Menu(SDL_Renderer* renderer, int screenWidth, int screenHeight);
    ~Menu();

    void render();
    MenuAction handleInput();
    MenuState getState() const { return currentState; }
    void setState(MenuState state) { 
        currentState = state; 
        if (state == MenuState::PAUSE_MENU || state == MenuState::MAIN_MENU) {
            selectedIndex = 0; // Reset selection when switching menus
        }
    }
    void setSelectedIndex(int index) { selectedIndex = index; }
    
    bool isInMenu() const { return currentState == MenuState::MAIN_MENU || currentState == MenuState::OPTIONS; }

private:
    SDL_Renderer* renderer;
    int screenWidth;
    int screenHeight;
    
    TTF_Font* font;
    TTF_Font* titleFont;
    
    MenuState currentState;
    int selectedIndex;
    std::vector<std::string> menuItems;
    
    void renderText(const std::string& text, int x, int y, TTF_Font* font, SDL_Color color);
    SDL_Texture* createTextTexture(const std::string& text, TTF_Font* font, SDL_Color color);
};

