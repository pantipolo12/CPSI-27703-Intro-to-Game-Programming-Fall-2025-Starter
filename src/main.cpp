#include "Game.h"
#define SDL_MAIN_HANDLED 
#ifdef _WIN32
#include <SDL.h>
#endif

int main(int argc, char* argv[]) {
    Game g;
    return g.run();
}
