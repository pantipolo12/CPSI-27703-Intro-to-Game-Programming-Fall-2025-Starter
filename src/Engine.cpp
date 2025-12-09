#include "Engine.h"
#include "Object.h"
#include "ImageDevice.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <algorithm>
#include <box2d/box2d.h>
#include <box2d/collision.h>
#include "InputDevice.h"
#include "BodyComponent.h"
#include "CharacterComponent.h"
#include "SpriteComponent.h"
#include "GroundComponent.h"
#include "LevelLoader.h"
#include "KeyComponent.h"
#include "DoorComponent.h"
#include "HealthComponent.h"
#include "MissileComponent.h"
#include "ImageDevice.h"

Engine* Engine::E = nullptr;

Engine::Engine() {

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    width = 800;
    height = 600;
    window = SDL_CreateWindow("Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_GetWindowSize(window, &width, &height);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    E = this;
    

    
    // Create Box2D world
    // Box2D uses Y-up coordinate system internally
    // We convert coordinates at the BodyComponent interface
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{0.0f, -400.0f};  // Realistic gravity (negative Y = down in Box2D's Y-up system)
    // Note: Box2D uses meters, but we're using pixels, so we scale gravity accordingly
    worldId = b2CreateWorld(&worldDef);
}
Engine::~Engine() {
    // Destroy Box2D world
    if (B2_IS_NON_NULL(worldId))
        b2DestroyWorld(worldId);
    
    //ImageDevice::cleanup();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    IMG_Quit();
    SDL_Quit();
}

Object* Engine::addObject() {
    objects.push_back(std::make_unique<Object>());
    return objects.back().get();
}

void Engine::update() {
    processInput();
    
    // Step physics world with fixed timestep (for consistent physics)
    if (B2_IS_NON_NULL(worldId)) {
        const float timeStep = 1.0f / 60.0f;  // Fixed timestep at 60 FPS
        const int subStepCount = 4;
        b2World_Step(worldId, timeStep, subStepCount);
        
        // Process contact events after physics step
        processContactEvents();
    }
    
    updateObjects();

    debugPlayerPosition(player);
    // Update camera
    updateView(player);
    render();
}

void Engine::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_WINDOW_RESIZABLE){
            width = event.window.data1;
            height = event.window.data2;
        }
        if (event.type == SDL_QUIT) {
            exit(0);
        }
        InputDevice::process(event);
    }
}

void Engine::updateObjects() {
    // Update all objects
    for (auto& obj : objects) 
        obj->update(this->dt); 
        
}

void Engine::render()
{
    if (!renderer) return;

    // Clear screen (black background)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render all objects
    for (auto& obj : objects) {
        obj->render();  // pass the camera view
        BodyComponent* body = obj->getComponent<BodyComponent>();
        // if (body) {
        //     drawRect(body->getX(), body->getY(), body->getWidth(), body->getHeight(), 255, 0, 0, 150);
        // }
    }
    
    // Draw raycast visualizations
    for (const auto& ray : raycastVisuals) {
        SDL_Rect startRect = view.transform(SDL_Rect{
            int(ray.x1 - 3), int(ray.y1 - 3), 6, 6
        });
        SDL_Rect endRect = view.transform(SDL_Rect{
            int(ray.x2 - 3), int(ray.y2 - 3), 6, 6
        });
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow for ray
        SDL_RenderFillRect(renderer, &startRect);
        
        if (ray.hit) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for hit point
            // Convert Box2D Y-up coordinate to SDL Y-down coordinate
            float hitY = view.worldHeight - ray.hitPoint.y;
            SDL_Rect hitRect = view.transform(SDL_Rect{
                int(ray.hitPoint.x - 5), int(hitY - 5), 10, 10
            });
            SDL_RenderFillRect(renderer, &hitRect);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for no hit
        }
        SDL_RenderFillRect(renderer, &endRect);
        
        // Draw line
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 128);
        SDL_Point start = {int(ray.x1 - view.x), int(ray.y1 - view.y)};
        SDL_Point end = {int(ray.x2 - view.x), int(ray.y2 - view.y)};
        SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
    }
    
    // Draw AABB query visualizations
    for (const auto& aabb : aabbQueryVisuals) {
        drawRect(aabb.x, aabb.y, aabb.w, aabb.h, 0, 255, 255, 100); // Cyan outline
    }

    // Optional: draw the ground line
    drawRect(0, groundY, view.worldWidth, 2, 0, 255, 0);

    // Note: Health UI and Game Over are rendered in main.cpp's render loop
    // Present the final frame
    SDL_RenderPresent(renderer);
}

void Engine::setView(int x, int y) {
    view.x = x;
    view.y = y;
}
void Engine::updateView(Object* player) {
    if (!player) return;

    BodyComponent* body = player->getComponent<BodyComponent>();
    if (!body) return;

    float px = body->getX() + body->getWidth() / 2;
    float py = body->getY() + body->getHeight() / 2;

    // Use View's method to center camera
    view.centerOn(px, py);
}

void Engine::drawRect(float x, float y, float w, float h, int r, int g, int b, int a)
{
    if (!renderer) return;

    // Make an SDL_Rect in world space
    SDL_Rect rect;
    rect.x = int(x);
    rect.y = int(y);
    rect.w = int(w);
    rect.h = int(h);

    // Apply camera transform
    rect = view.transform(rect);

    // Draw the rectangle
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
}

void Engine::drawImage(std::string textureName, float x, float y, float w, float h, float angle, bool centerOrigin ) {
    SDL_Texture* tex = ImageDevice::get(textureName);
    SDL_Rect rect;
    SDL_Point* pCenter = nullptr;
    
    if (centerOrigin) {
        rect = {(int)(x - view.x - w / 2), (int)(y - view.y - h / 2), (int)w, (int)h};
        static SDL_Point centerPoint; 
        centerPoint = {(int)(w/2), (int)(h/2)};
        pCenter = &centerPoint;
    } else {
        rect = {(int)(x - view.x), (int)(y - view.y), (int)w, (int)h};
        pCenter = nullptr; // top-left origin
    }

    SDL_RenderCopyEx(renderer, tex, nullptr, &rect, angle, pCenter, SDL_FLIP_NONE);
}

void Engine::debugDrawObjects() {
    if (!renderer) return;

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // bright green

    for (auto& obj : objects) {
        BodyComponent* body = obj->getComponent<BodyComponent>();
        if (!body) continue;

        // World rectangle
        SDL_Rect worldRect = {
            int(body->getX()),
            int(body->getY()),
            int(body->getWidth()),
            int(body->getHeight())
        };

        // Transform with camera
        SDL_Rect screenRect = view.transform(worldRect);

        SDL_RenderDrawRect(renderer, &screenRect);
    }
}

void Engine::debugPlayerPosition(Object* player) {
    if (!player) return;
    BodyComponent* body = player->getComponent<BodyComponent>();
    if (!body) return;

    float px = body->getX();
    float py = body->getY();

    std::cout << "Player Position -> X: " << px
              << ", Y: " << py
              << " | Camera -> X: " << view.x
              << ", Y: " << view.y
              << std::endl;
}

Object* Engine::findObjectById(const std::string& id)
{
    for (auto& obj : objects)
        if (obj->getId() == id)
            return obj.get();  
    return nullptr;
}

void Engine::update(float dt) {
    this->dt = dt;
    
    // Check if a level load was queued (do this first, before any updates)
    if (hasQueuedLevel) {
        std::string levelToLoad = queuedLevelPath;
        hasQueuedLevel = false;
        queuedLevelPath.clear();
        std::cout << "[ENGINE] Processing queued level load: " << levelToLoad << std::endl;
        loadLevel(levelToLoad);
        return; // Don't update this frame, let the new level initialize
    }
    
    // Step physics world with variable timestep
    if (B2_IS_NON_NULL(worldId)) {
        const int subStepCount = 4;
        b2World_Step(worldId, dt, subStepCount);
        
        // Process contact events after physics step
        processContactEvents();
    }
    
    // Handle interactive physics controls
    handlePhysicsControls();
    
    // Update raycast and AABB query visualizations
    for (auto& ray : raycastVisuals) {
        ray.lifetime -= dt;
    }
    raycastVisuals.erase(
        std::remove_if(raycastVisuals.begin(), raycastVisuals.end(),
            [](const RaycastVisual& r) { return r.lifetime <= 0; }),
        raycastVisuals.end());
    
    for (auto& aabb : aabbQueryVisuals) {
        aabb.lifetime -= dt;
    }
    aabbQueryVisuals.erase(
        std::remove_if(aabbQueryVisuals.begin(), aabbQueryVisuals.end(),
            [](const AABBQueryVisual& a) { return a.lifetime <= 0; }),
        aabbQueryVisuals.end());
    
    // Manual bee collision check (runs every frame to ensure collisions are detected)
    // Only check if player is alive
    if (player) {
        HealthComponent* health = player->getComponent<HealthComponent>();
        if (health && !health->isDead()) {
            checkBeeCollisions();
        }
    }
    
    for(auto& obj : getObjects()) {
        if (auto* charComp = obj->getComponent<CharacterComponent>()) {
            charComp->update(dt);
        }
        // Update KeyComponent and DoorComponent
        if (auto* keyComp = obj->getComponent<KeyComponent>()) {
            keyComp->update(dt);
        }
        if (auto* doorComp = obj->getComponent<DoorComponent>()) {
            doorComp->update(dt);
        }
        if (auto* healthComp = obj->getComponent<HealthComponent>()) {
            healthComp->update(dt);
        }
    }
}

// Static callback function for raycast (required because lambdas with captures can't convert to function pointers)
static float RaycastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* ctx) {
    struct RaycastContext {
        Engine* engine;
        Engine::RaycastResult* result;
    };
    RaycastContext* ctxt = static_cast<RaycastContext*>(ctx);
    
    // Get the body from the shape
    b2BodyId bodyId = b2Shape_GetBody(shapeId);
    void* userData = b2Body_GetUserData(bodyId);
    
    if (userData) {
        Object* obj = static_cast<Object*>(userData);
        ctxt->result->hit = true;
        ctxt->result->point = point;
        ctxt->result->normal = normal;
        ctxt->result->fraction = fraction;
        ctxt->result->object = obj;
        
        // Return fraction to clip the ray (stop at first hit)
        return fraction;
    }
    
    return -1.0f; // Ignore shapes without userData
}

// Raycast implementation
Engine::RaycastResult Engine::castRay(float x1, float y1, float x2, float y2) {
    RaycastResult result;
    result.hit = false;
    result.object = nullptr;
    
    if (!B2_IS_NON_NULL(worldId)) return result;
    
    // Convert SDL coordinates to Box2D coordinates
    b2Vec2 origin = b2Vec2{x1, sdlToBox2DY(y1)};
    b2Vec2 target = b2Vec2{x2, sdlToBox2DY(y2)};
    b2Vec2 translation = b2Vec2{target.x - origin.x, target.y - origin.y};
    
    // Default query filter (no filtering)
    b2QueryFilter filter = b2DefaultQueryFilter();
    
    struct RaycastContext {
        Engine* engine;
        RaycastResult* result;
    } context = {this, &result};
    
    b2World_CastRay(worldId, origin, translation, filter, RaycastCallback, &context);
    
    // Add visualization
    RaycastVisual visual;
    visual.x1 = x1;
    visual.y1 = y1;
    visual.x2 = x2;
    visual.y2 = y2;
    visual.hit = result.hit;
    if (result.hit) {
        visual.hitPoint = result.point;
    }
    visual.lifetime = 2.0f; // Show for 2 seconds
    raycastVisuals.push_back(visual);
    
    return result;
}

// Static callback function for AABB query (required because lambdas with captures can't convert to function pointers)
static bool AABBQueryCallback(b2ShapeId shapeId, void* ctx) {
    struct QueryContext {
        Engine* engine;
        Engine::AABBQueryResult* result;
    };
    QueryContext* ctxt = static_cast<QueryContext*>(ctx);
    
    // Get the body from the shape
    b2BodyId bodyId = b2Shape_GetBody(shapeId);
    void* userData = b2Body_GetUserData(bodyId);
    
    if (userData) {
        Object* obj = static_cast<Object*>(userData);
        ctxt->result->objects.push_back(obj);
    }
    
    return true; // Continue query
}

// AABB Query implementation
Engine::AABBQueryResult Engine::queryAABB(float x, float y, float width, float height) {
    AABBQueryResult result;
    
    if (!B2_IS_NON_NULL(worldId)) return result;
    
    // Convert SDL coordinates to Box2D coordinates
    // Box2D uses center and half-widths
    float centerX = x + width / 2.0f;
    float centerY = sdlToBox2DY(y + height / 2.0f);
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    
    b2AABB aabb;
    aabb.lowerBound = b2Vec2{centerX - halfWidth, centerY - halfHeight};
    aabb.upperBound = b2Vec2{centerX + halfWidth, centerY + halfHeight};
    
    // Default query filter (no filtering)
    b2QueryFilter filter = b2DefaultQueryFilter();
    
    struct QueryContext {
        Engine* engine;
        AABBQueryResult* result;
    } context = {this, &result};
    
    b2World_OverlapAABB(worldId, aabb, filter, AABBQueryCallback, &context);
    
    // Add visualization
    AABBQueryVisual visual;
    visual.x = x;
    visual.y = y;
    visual.w = width;
    visual.h = height;
    visual.lifetime = 1.0f; // Show for 1 second
    aabbQueryVisuals.push_back(visual);
    
    return result;
}

// Manual collision check for bees (backup to contact events)
// Player dies after 3 bee collisions (health starts at 3, each collision does 1 damage)
void Engine::checkBeeCollisions() {
    if (!player) return;
    
    BodyComponent* playerBody = player->getComponent<BodyComponent>();
    if (!playerBody) return;
    
    HealthComponent* health = player->getComponent<HealthComponent>();
    if (!health) {
        std::cout << "[BEE COLLISION] WARNING: Player has no HealthComponent!" << std::endl;
        return;
    }
    if (health->isDead()) {
        // Player is already dead, don't check collisions
        return;
    }
    
    float playerX = playerBody->getX();
    float playerY = playerBody->getY();
    float playerW = playerBody->getWidth();
    float playerH = playerBody->getHeight();
    
    float playerLeft = playerX - playerW / 2;
    float playerRight = playerX + playerW / 2;
    float playerTop = playerY - playerH / 2;
    float playerBottom = playerY + playerH / 2;
    
    // Check all objects for bees
    for (auto& obj : objects) {
        if (obj.get() == player) continue;
        
        // Check if this is a bee
        bool isBee = (obj->getComponent<MissileComponent>() != nullptr) || 
                      (obj->getId().find("bee") != std::string::npos);
        
        if (isBee) {
            BodyComponent* beeBody = obj->getComponent<BodyComponent>();
            if (beeBody) {
                float beeX = beeBody->getX();
                float beeY = beeBody->getY();
                float beeW = beeBody->getWidth();
                float beeH = beeBody->getHeight();
                
                float beeLeft = beeX - beeW / 2;
                float beeRight = beeX + beeW / 2;
                float beeTop = beeY - beeH / 2;
                float beeBottom = beeY + beeH / 2;
                
                // Check for overlap
                bool overlapX = (playerRight > beeLeft) && (playerLeft < beeRight);
                bool overlapY = (playerBottom > beeTop) && (playerTop < beeBottom);
                
                if (overlapX && overlapY) {
                    // Manual collision detected
                    const float BEE_DAMAGE_COOLDOWN = 1.0f; // 1 second cooldown
                    float currentTime = SDL_GetTicks() / 1000.0f;
                    
                    auto it = beeDamageCooldown.find(obj.get());
                    bool onCooldown = (it != beeDamageCooldown.end() && currentTime < it->second);
                    
                    if (!onCooldown) {
                        int healthBefore = health->getHealth();
                        
                        // Apply damage - this will check invulnerability internally
                        health->takeDamage(1);
                        int healthAfter = health->getHealth();
                        
                        // Always set cooldown to prevent spam (even if damage was blocked by invulnerability)
                        beeDamageCooldown[obj.get()] = currentTime + BEE_DAMAGE_COOLDOWN;
                        
                        // Check if player died
                        if (health->isDead()) {
                            std::cout << "[GAME OVER] *** PLAYER DIED AFTER " << (3 - healthAfter) << " BEE COLLISIONS! ***" << std::endl;
                        }
                    }
                }
            }
        }
    }
}

// Contact event processing
void Engine::processContactEvents() {
    if (!B2_IS_NON_NULL(worldId) || !player) return;
    
    b2ContactEvents events = b2World_GetContactEvents(worldId);
    
    // Process begin contact events
    for (int i = 0; i < events.beginCount; i++) {
        const b2ContactBeginTouchEvent& event = events.beginEvents[i];
        
        // Get objects from shapes
        b2BodyId bodyA = b2Shape_GetBody(event.shapeIdA);
        b2BodyId bodyB = b2Shape_GetBody(event.shapeIdB);
        
        void* userDataA = b2Body_GetUserData(bodyA);
        void* userDataB = b2Body_GetUserData(bodyB);
        
        if (userDataA && userDataB) {
            Object* objA = static_cast<Object*>(userDataA);
            Object* objB = static_cast<Object*>(userDataB);
            
            // Check if player was hit by bee
            Object* playerObj = player;
            Object* beeObj = nullptr;
            
            if (objA == playerObj || objB == playerObj) {
                Object* otherObj = (objA == playerObj) ? objB : objA;
                // Check if other object is a bee (has MissileComponent or id contains "bee")
                if (otherObj->getComponent<MissileComponent>() || 
                    otherObj->getId().find("bee") != std::string::npos) {
                    beeObj = otherObj;
                }
            }
            
            if (beeObj && playerObj) {
                HealthComponent* health = playerObj->getComponent<HealthComponent>();
                if (health && !health->isDead()) {
                    // Check if this bee has recently damaged the player (cooldown to prevent multiple hits)
                    const float BEE_DAMAGE_COOLDOWN = 1.0f; // 1 second cooldown per bee (matches invulnerability)
                    float currentTime = SDL_GetTicks() / 1000.0f; // Convert to seconds
                    
                    // Check cooldown
                    auto it = beeDamageCooldown.find(beeObj);
                    bool onCooldown = (it != beeDamageCooldown.end() && currentTime < it->second);
                    
                    if (!onCooldown) {
                        // Apply damage (HealthComponent will check its own invulnerability)
                        int healthBefore = health->getHealth();
                        
                        health->takeDamage(1);
                        int healthAfter = health->getHealth();
                        
                        // Always set cooldown to prevent spam
                        beeDamageCooldown[beeObj] = currentTime + BEE_DAMAGE_COOLDOWN;
                        
                        // Log if damage was actually applied
                        if (healthBefore != healthAfter) {
                            std::cout << "[DAMAGE] Player hit by bee (" << beeObj->getId() << ")! Health: " 
                                      << healthAfter << "/" << health->getMaxHealth() << std::endl;
                            
                            // Check if player is dead
                            if (health->isDead()) {
                                std::cout << "[GAME OVER] *** PLAYER IS DEAD! ***" << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Process end contact events
    for (int i = 0; i < events.endCount; i++) {
        const b2ContactEndTouchEvent& event = events.endEvents[i];
        
        // Check if shapes are still valid
        if (!b2Shape_IsValid(event.shapeIdA) || !b2Shape_IsValid(event.shapeIdB)) {
            continue;
        }
        
        b2BodyId bodyA = b2Shape_GetBody(event.shapeIdA);
        b2BodyId bodyB = b2Shape_GetBody(event.shapeIdB);
        
        void* userDataA = b2Body_GetUserData(bodyA);
        void* userDataB = b2Body_GetUserData(bodyB);
        
        if (userDataA && userDataB) {
            Object* objA = static_cast<Object*>(userDataA);
            Object* objB = static_cast<Object*>(userDataB);
            
            std::cout << "[CONTACT END] " << objA->getId() << " separated from " << objB->getId() << std::endl;
        }
    }
    
    // Process hit events (high-speed collisions)
    for (int i = 0; i < events.hitCount; i++) {
        const b2ContactHitEvent& event = events.hitEvents[i];
        
        b2BodyId bodyA = b2Shape_GetBody(event.shapeIdA);
        b2BodyId bodyB = b2Shape_GetBody(event.shapeIdB);
        
        void* userDataA = b2Body_GetUserData(bodyA);
        void* userDataB = b2Body_GetUserData(bodyB);
        
        if (userDataA && userDataB) {
            Object* objA = static_cast<Object*>(userDataA);
            Object* objB = static_cast<Object*>(userDataB);
            
            std::cout << "[CONTACT HIT] " << objA->getId() << " hit " << objB->getId() 
                      << " at speed " << event.approachSpeed << std::endl;
        }
    }
}

// Runtime body creation
Object* Engine::createDynamicBody(float x, float y, float w, float h) {
    Object* obj = addObject();
    obj->addComponent<BodyComponent>(worldId, x, y, w, h, true, view.worldHeight);
    obj->initializeBodyComponentUserData(); // Initialize userData
    obj->addComponent<SpriteComponent>(255, 200, 0); // Orange color for dynamic bodies
    return obj;
}

Object* Engine::createStaticBody(float x, float y, float w, float h) {
    Object* obj = addObject();
    obj->addComponent<BodyComponent>(worldId, x, y, w, h, false, view.worldHeight);
    obj->initializeBodyComponentUserData(); // Initialize userData
    obj->addComponent<SpriteComponent>(128, 128, 128); // Gray color for static bodies
    return obj;
}

void Engine::removeObject(Object* obj) {
    if (!obj) return;
    
    // Find and remove from objects vector
    objects.erase(
        std::remove_if(objects.begin(), objects.end(),
            [obj](const std::unique_ptr<Object>& ptr) { return ptr.get() == obj; }),
        objects.end());
}

void Engine::removeObjectById(const std::string& id) {
    Object* obj = findObjectById(id);
    if (obj) {
        removeObject(obj);
    }
}

void Engine::queueLevelLoad(const std::string& levelPath) {
    queuedLevelPath = levelPath;
    hasQueuedLevel = true;
    std::cout << "[ENGINE] Level load queued: " << levelPath << std::endl;
}

void Engine::loadLevel(const std::string& levelPath) {
    std::cout << "[ENGINE] ========================================" << std::endl;
    std::cout << "[ENGINE] Loading level: " << levelPath << std::endl;
    
    // Clear bee damage cooldowns first
    beeDamageCooldown.clear();
    
    // Clear all current objects (this will destroy their bodies via destructors)
    // Important: Clear objects before loading new ones to avoid conflicts
    int oldObjectCount = objects.size();
    objects.clear();
    player = nullptr;
    std::cout << "[ENGINE] Cleared " << oldObjectCount << " old objects" << std::endl;
    
    // Load the new level
    if (LevelLoader::load(levelPath, *this)) {
        // Set world size (you may want to make this configurable per level)
        setWorldSize(5000, 1200);
        std::cout << "[ENGINE] Level loaded successfully: " << levelPath << std::endl;
        std::cout << "[ENGINE] New object count: " << objects.size() << std::endl;
        
        // Reset view to center on player if player exists
        if (player) {
            BodyComponent* playerBody = player->getComponent<BodyComponent>();
            if (playerBody) {
                float playerX = playerBody->getX();
                float playerY = playerBody->getY();
                updateView(player);
                std::cout << "[ENGINE] View updated to player position: (" << playerX << ", " << playerY << ")" << std::endl;
                
                // Verify health component
                HealthComponent* health = player->getComponent<HealthComponent>();
                if (health) {
                    std::cout << "[ENGINE] Player health: " << health->getHealth() << "/" << health->getMaxHealth() << std::endl;
                } else {
                    std::cout << "[ENGINE] WARNING: Player has no HealthComponent!" << std::endl;
                }
            }
        } else {
            std::cout << "[ENGINE] WARNING: No player found after level load!" << std::endl;
        }
        std::cout << "[ENGINE] ========================================" << std::endl;
    } else {
        std::cerr << "[ENGINE] ERROR: Failed to load level: " << levelPath << std::endl;
        std::cerr << "[ENGINE] ========================================" << std::endl;
    }
}

void Engine::renderHealthUI() {
    if (!player) return;
    
    HealthComponent* health = player->getComponent<HealthComponent>();
    if (!health) return;
    
    int currentHealth = health->getHealth();
    int maxHealth = health->getMaxHealth();
    
    // Heart size
    const int heartSize = 32;
    const int heartSpacing = 5;
    const int startX = 20;
    const int startY = 50;
    
    SDL_Texture* heartTex = ImageDevice::get("heart");
    if (!heartTex) return;
    
    // Draw hearts (screen space, not affected by camera)
    // Reset texture color mod first to ensure clean state
    SDL_SetTextureColorMod(heartTex, 255, 255, 255);
    
    for (int i = 0; i < maxHealth; i++) {
        SDL_Rect heartRect;
        heartRect.x = startX + i * (heartSize + heartSpacing);
        heartRect.y = startY;
        heartRect.w = heartSize;
        heartRect.h = heartSize;
        
        // Draw filled heart if player has this life, otherwise draw empty/dark
        if (i < currentHealth) {
            // Full color heart
            SDL_SetTextureColorMod(heartTex, 255, 255, 255);
            SDL_RenderCopy(renderer, heartTex, nullptr, &heartRect);
        } else {
            // Draw darkened heart
            SDL_SetTextureColorMod(heartTex, 100, 100, 100); // Darken
            SDL_RenderCopy(renderer, heartTex, nullptr, &heartRect);
        }
    }
    
    // Reset texture color mod after rendering
    SDL_SetTextureColorMod(heartTex, 255, 255, 255);
}

void Engine::renderGameOver() {
    if (!renderer) return;
    
    // Draw semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, width, height};
    SDL_RenderFillRect(renderer, &overlay);
    
    // Note: For text rendering, you would need SDL_ttf
    // For now, we'll use simple rectangles to represent text/button
    // You can enhance this later with actual text rendering
    
    // "Game Over" text area (represented as a rectangle for now)
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect gameOverRect = {width / 2 - 100, height / 2 - 100, 200, 50};
    SDL_RenderFillRect(renderer, &gameOverRect);
    
    // "Try Again" button
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect buttonRect = {width / 2 - 75, height / 2, 150, 40};
    SDL_RenderFillRect(renderer, &buttonRect);
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

bool Engine::isGameOver() const {
    if (!player) {
        return false;
    }
    HealthComponent* health = player->getComponent<HealthComponent>();
    if (!health) {
        return false;
    }
    bool dead = health->isDead();
    if (dead) {
        std::cout << "[GAME OVER] Game Over! Player health: " << health->getHealth() << "/" << health->getMaxHealth() << std::endl;
    }
    return dead;
}

void Engine::resetGame() {
    // Reload the level
    loadLevel("assets/level.xml");
}

// Interactive controls for testing physics features
void Engine::handlePhysicsControls() {
    static bool keyStates[10] = {false}; // Track key press states to avoid repeat
    
    // F key: Apply force to player
    if (InputDevice::isKeyDown(SDL_SCANCODE_F)) {
        if (!keyStates[0] && player) {
            BodyComponent* body = player->getComponent<BodyComponent>();
            if (body) {
                body->applyForceToCenter(b2Vec2{500.0f, 0.0f}); // Push right
                std::cout << "[PHYSICS] Applied force to player" << std::endl;
                keyStates[0] = true;
            }
        }
    } else {
        keyStates[0] = false;
    }
    
    // G key: Apply impulse to player
    if (InputDevice::isKeyDown(SDL_SCANCODE_G)) {
        if (!keyStates[1] && player) {
            BodyComponent* body = player->getComponent<BodyComponent>();
            if (body) {
                body->applyLinearImpulseToCenter(b2Vec2{0.0f, -300.0f}); // Jump impulse
                std::cout << "[PHYSICS] Applied impulse to player" << std::endl;
                keyStates[1] = true;
            }
        }
    } else {
        keyStates[1] = false;
    }
    
    // R key: Set angular velocity on player
    if (InputDevice::isKeyDown(SDL_SCANCODE_R)) {
        if (!keyStates[2] && player) {
            BodyComponent* body = player->getComponent<BodyComponent>();
            if (body) {
                body->setAngularVelocity(2.0f); // Rotate
                std::cout << "[PHYSICS] Set angular velocity on player" << std::endl;
                keyStates[2] = true;
            }
        }
    } else {
        keyStates[2] = false;
    }
    
    // T key: Cast ray from player
    if (InputDevice::isKeyDown(SDL_SCANCODE_T)) {
        if (!keyStates[3] && player) {
            BodyComponent* body = player->getComponent<BodyComponent>();
            if (body) {
                float px = body->getX();
                float py = body->getY();
                // Cast ray forward from player
                RaycastResult result = castRay(px, py, px + 500, py);
                if (result.hit) {
                    std::cout << "[RAYCAST] Hit " << (result.object ? result.object->getId() : "unknown") 
                              << " at fraction " << result.fraction << std::endl;
                } else {
                    std::cout << "[RAYCAST] No hit" << std::endl;
                }
                keyStates[3] = true;
            }
        }
    } else {
        keyStates[3] = false;
    }
    
    // Q key: AABB query around player
    if (InputDevice::isKeyDown(SDL_SCANCODE_Q)) {
        if (!keyStates[4] && player) {
            BodyComponent* body = player->getComponent<BodyComponent>();
            if (body) {
                float px = body->getX();
                float py = body->getY();
                AABBQueryResult result = queryAABB(px - 200, py - 200, 400, 400);
                std::cout << "[AABB QUERY] Found " << result.objects.size() << " objects" << std::endl;
                for (Object* obj : result.objects) {
                    if (obj != player) {
                        std::cout << "  - " << obj->getId() << std::endl;
                    }
                }
                keyStates[4] = true;
            }
        }
    } else {
        keyStates[4] = false;
    }
    
    // 1 key: Spawn dynamic body
    if (InputDevice::isKeyDown(SDL_SCANCODE_1)) {
        if (!keyStates[5]) {
            float spawnX = view.x + width / 2;
            float spawnY = view.y + 100;
            Object* newObj = createDynamicBody(spawnX, spawnY, 50, 50);
            newObj->setId("dynamic_" + std::to_string(objects.size()));
            std::cout << "[SPAWN] Created dynamic body at (" << spawnX << ", " << spawnY << ")" << std::endl;
            keyStates[5] = true;
        }
    } else {
        keyStates[5] = false;
    }
    
    // 2 key: Spawn static body
    if (InputDevice::isKeyDown(SDL_SCANCODE_2)) {
        if (!keyStates[6]) {
            float spawnX = view.x + width / 2;
            float spawnY = view.y + 200;
            Object* newObj = createStaticBody(spawnX, spawnY, 100, 50);
            newObj->setId("static_" + std::to_string(objects.size()));
            std::cout << "[SPAWN] Created static body at (" << spawnX << ", " << spawnY << ")" << std::endl;
            keyStates[6] = true;
        }
    } else {
        keyStates[6] = false;
    }
    
    // X key: Delete last spawned object (non-player, non-ground)
    if (InputDevice::isKeyDown(SDL_SCANCODE_X)) {
        if (!keyStates[7]) {
            // Find last object that's not player or ground
            for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
                Object* obj = it->get();
                if (obj != player && !obj->getComponent<GroundComponent>()) {
                    std::string id = obj->getId();
                    removeObject(obj);
                    std::cout << "[DELETE] Removed object " << id << std::endl;
                    break;
                }
            }
            keyStates[7] = true;
        }
    } else {
        keyStates[7] = false;
    }
}
