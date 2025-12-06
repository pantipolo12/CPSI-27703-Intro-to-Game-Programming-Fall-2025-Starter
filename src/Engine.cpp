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
        obj->update(dt); 
        
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
    
    for(auto& obj : getObjects()) {
        if (auto* charComp = obj->getComponent<CharacterComponent>()) {
            charComp->update(dt);
        }
        // other updates
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

// Contact event processing
void Engine::processContactEvents() {
    if (!B2_IS_NON_NULL(worldId)) return;
    
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
            
            std::cout << "[CONTACT BEGIN] " << objA->getId() << " collided with " << objB->getId() << std::endl;
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
