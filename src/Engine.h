#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <SDL.h>
#include <box2d/box2d.h>
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
        b2WorldId getWorldId() const { return worldId; }
        void loadLevel(const std::string& levelPath); // Load a new level
        void queueLevelLoad(const std::string& levelPath); // Queue a level load for next frame
        void renderHealthUI(); // Render health hearts on screen
        void renderGameOver(); // Render game over screen
        bool isGameOver() const; // Check if game is over
        void resetGame(); // Reset game state
        
        // Physics queries
        struct RaycastResult {
            bool hit;
            b2Vec2 point;
            b2Vec2 normal;
            float fraction;
            Object* object;
        };
        RaycastResult castRay(float x1, float y1, float x2, float y2);
        
        struct AABBQueryResult {
            std::vector<Object*> objects;
        };
        AABBQueryResult queryAABB(float x, float y, float width, float height);
        
        // Contact handling
        void processContactEvents();
        void checkBeeCollisions(); // Manual collision check for bees
        
        // Runtime body management
        Object* createDynamicBody(float x, float y, float w, float h);
        Object* createStaticBody(float x, float y, float w, float h);
        void removeObject(Object* obj);
        void removeObjectById(const std::string& id);
        
        // Interactive controls (for demo)
        void handlePhysicsControls();
  
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
    
    // Box2D world
    b2WorldId worldId{};
    
    // Contact event tracking
    int lastContactBeginCount = 0;
    int lastContactEndCount = 0;
    int lastContactHitCount = 0;
    
    // Track which bees have recently damaged the player (to prevent multiple hits from same collision)
    std::unordered_map<Object*, float> beeDamageCooldown; // Bee object -> time until can damage again
    
    // Level loading queue (to avoid crashes when loading during update)
    std::string queuedLevelPath; // Level path to load on next frame
    bool hasQueuedLevel = false;
    
    // Raycast visualization
    struct RaycastVisual {
        float x1, y1, x2, y2;
        bool hit;
        b2Vec2 hitPoint;
        float lifetime;
    };
    std::vector<RaycastVisual> raycastVisuals;
    
    // AABB query visualization
    struct AABBQueryVisual {
        float x, y, w, h;
        float lifetime;
    };
    std::vector<AABBQueryVisual> aabbQueryVisuals;
    
    // Internal methods
    void processInput();
    //void updateView();
    void updateObjects();
    void render();
    
    // Helper for coordinate conversion
    float sdlToBox2DY(float sdlY) const { return view.worldHeight - sdlY; }
    float box2DToSDLY(float box2DY) const { return view.worldHeight - box2DY; }
};
