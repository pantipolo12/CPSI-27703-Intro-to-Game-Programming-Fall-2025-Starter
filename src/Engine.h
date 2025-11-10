#pragma once
#include <vector>
#include <memory>
#include <SDL.h>
#include "Object.h"
#include "View.h"

class Engine {
public:
    static Engine* E;
    View& getView() { return view; }
    Engine();
    ~Engine();

    // Core engine methods
    Object* addObject();
    void setView(int x, int y);
    void update();
    void render(const View& view);
    SDL_Renderer* getRenderer(){return renderer;}
    
    // Screen dimensions
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    // Fixed ground
    void setGroundY(float y) { groundY = y; }
    float getGroundY() const { return groundY; }

        
    static void drawRect(float x, float y, float width, float height, int r, int g, int b, int a=255);
    static void drawImage( std::string textureName, float x=0, float y=0, float width=100, float height=100, float angle=0  );

    Object* getObject(int index){return objects[index].get();}
    Object* getLastObject(){return getObject(objects.size()-1);}

    std::vector<std::unique_ptr<Object>>& getObjects() { return objects; }

    void followPlayer(Object* player);

    void updateView(Object* player);
private:
    std::vector<std::unique_ptr<Object>> objects;
    SDL_Window* window;
    SDL_Renderer* renderer;
    View view;
    int width;
    int height;

    // Ground level (Y coordinate of the top of the ground)
    float groundY{600}; // Default bottom of window
    
    // Internal methods
    void processInput();
    void updateObjects();
    void render();
};
