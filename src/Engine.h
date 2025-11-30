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
        //void getView() {return view};
        void update();
        void update(float dt);
        void render(const View& view);
        SDL_Renderer* getRenderer(){return renderer;}
        
        // Screen dimensions
        int getWidth() const { return width; }
        int getHeight() const { return height; }
        
        // Fixed ground
        void setGroundY(float y) { groundY = y; }
        float getGroundY() const { return groundY; }
    
            
        void drawRect(float x, float y, float width, float height, int r, int g, int b, int a=255);
        //static void drawImage( std::string textureName, float x=0, float y=0, float width=100, float height=100, float angle=0  );
        void drawImage( std::string textureName, float x=0, float y=0, float width=100, float height=100, float angle=0, bool centerOrigin = true);
    
        Object* getObject(int index){return objects[index].get();}
        Object* getLastObject(){return getObject(objects.size()-1);}
    
        std::vector<std::unique_ptr<Object>>& getObjects() { return objects; }
    
        void followPlayer(Object* player);
    
        void updateView(Object* player);
        void setPlayer(Object* p) { player = p; }
        void setWorldSize(int w, int h) { view.worldWidth = w; view.worldHeight = h; }
        void debugDrawObjects();
        int getWorldWidth() const { return view.worldWidth;}
        int getWorldHeight() const { return view.worldHeight;}
        void debugPlayerPosition(Object* player);
        Object* getPlayer() { return player; }  // player is already stored via setPlayer()
        Object* findObjectById(const std::string& id);
        void updateViewWithParallax(Object* player, float parallaxFactor);
  
private:
    Object* player = nullptr;
    std::vector<std::unique_ptr<Object>> objects;
    SDL_Window* window;
    SDL_Renderer* renderer;
    View view;
    int width;
    int height;
    float dt;

    // Ground level (Y coordinate of the top of the ground)
    float groundY{600}; // Default bottom of window
    
    // Internal methods
    void processInput();
    //void updateView();
    void updateObjects();
    void render();
};
