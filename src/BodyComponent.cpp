#include "BodyComponent.h"
#include "Object.h"
#include <box2d/box2d.h>
#include <box2d/math_functions.h>
#include <SDL.h>

BodyComponent::BodyComponent(b2WorldId world, float x, float y, float w, float h, bool isDynamic, float worldHeight)
    : world(world), width(w), height(h), worldHeight(worldHeight)
{
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = isDynamic ? b2_dynamicBody : b2_staticBody;
    // Convert SDL Y-down coordinates to Box2D Y-up coordinates
    bodyDef.position = b2Vec2{x, sdlToBox2DY(y)};
    body = b2CreateBody(world, &bodyDef);

    b2Polygon polygon = b2MakeBox(w / 2, h / 2);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.material.friction = 0.3f;
    // Enable contact events for dynamic and kinematic bodies
    if (isDynamic) {
        shapeDef.enableContactEvents = true;
        shapeDef.enableHitEvents = true;
    }
    shape = b2CreatePolygonShape(body, &shapeDef, &polygon);
    
    // Set userData to point to the Object (will be set after component is added to object)
    // Note: This will be called from Object::addComponent after setObject() is called
}

BodyComponent::~BodyComponent() {
    if (B2_IS_NON_NULL(body))
        b2DestroyBody(body);
}



// Position
float BodyComponent::getX() const {
    return b2Body_GetPosition(body).x;
}

float BodyComponent::getY() const {
    // Convert Box2D Y-up coordinates back to SDL Y-down coordinates
    return box2DToSDLY(b2Body_GetPosition(body).y);
}

void BodyComponent::setX(float x) {
    auto pos = b2Body_GetPosition(body);
    auto rot = b2Body_GetRotation(body);
    b2Body_SetTransform(body, b2Vec2{x, pos.y}, rot);
}

void BodyComponent::setY(float y) {
    auto pos = b2Body_GetPosition(body);
    auto rot = b2Body_GetRotation(body);
    // Convert SDL Y-down coordinates to Box2D Y-up coordinates
    b2Body_SetTransform(body, b2Vec2{pos.x, sdlToBox2DY(y)}, rot);
}

// Velocity
float BodyComponent::getVx() const {
    return b2Body_GetLinearVelocity(body).x;
}

float BodyComponent::getVy() const {
    // Invert Y velocity: Box2D Y-up to SDL Y-down
    return -b2Body_GetLinearVelocity(body).y;
}

void BodyComponent::setVx(float vx) {
    auto vel = b2Body_GetLinearVelocity(body);
    vel.x = vx;
    b2Body_SetLinearVelocity(body, vel);
}

void BodyComponent::setVy(float vy) {
    auto vel = b2Body_GetLinearVelocity(body);
    // Invert Y velocity: SDL Y-down means positive vy goes down,
    // but Box2D Y-up means positive vy goes up, so we need to negate
    vel.y = -vy;
    b2Body_SetLinearVelocity(body, vel);
}

void BodyComponent::setLinearVelocity(const b2Vec2& vel) {
    b2Body_SetLinearVelocity(body, vel);
}

b2Vec2 BodyComponent::getLinearVelocity() const {
    return b2Body_GetLinearVelocity(body);
}

// Force and impulse methods
void BodyComponent::applyForce(const b2Vec2& force, const b2Vec2& point) {
    // Convert point from SDL coordinates to Box2D coordinates
    b2Vec2 box2DPoint = b2Vec2{point.x, sdlToBox2DY(point.y)};
    b2Body_ApplyForce(body, force, box2DPoint, true); // wake = true
}

void BodyComponent::applyForceToCenter(const b2Vec2& force) {
    b2Body_ApplyForceToCenter(body, force, true); // wake = true
}

void BodyComponent::applyLinearImpulse(const b2Vec2& impulse, const b2Vec2& point) {
    // Convert point from SDL coordinates to Box2D coordinates
    b2Vec2 box2DPoint = b2Vec2{point.x, sdlToBox2DY(point.y)};
    b2Body_ApplyLinearImpulse(body, impulse, box2DPoint, true); // wake = true
}

void BodyComponent::applyLinearImpulseToCenter(const b2Vec2& impulse) {
    b2Body_ApplyLinearImpulseToCenter(body, impulse, true); // wake = true
}

// Angular velocity
float BodyComponent::getAngularVelocity() const {
    return b2Body_GetAngularVelocity(body);
}

void BodyComponent::setAngularVelocity(float angularVelocity) {
    b2Body_SetAngularVelocity(body, angularVelocity);
}

// Set userData when component is attached to object
void BodyComponent::initializeUserData() {
    // This should be called after the component is added to an object
    if (B2_IS_NON_NULL(body) && object) {
        b2Body_SetUserData(body, object);
    }
}

// Angle
float BodyComponent::getAngle() const {
    b2Rot rot = b2Body_GetRotation(body);
    return b2Rot_GetAngle(rot) * 180.0f / 3.14159265f;
}

void BodyComponent::setAngle(float angle) {
    auto pos = b2Body_GetPosition(body);
    b2Rot rot = b2MakeRot(angle * 3.14159265f / 180.0f);
    b2Body_SetTransform(body, pos, rot);
}

// SDL rectangle
SDL_Rect BodyComponent::getRect() const {
    float sdlX = getX();
    float sdlY = getY();
    return SDL_Rect{
        int(sdlX - width / 2),
        int(sdlY - height / 2),
        int(width),
        int(height)
    };
}
