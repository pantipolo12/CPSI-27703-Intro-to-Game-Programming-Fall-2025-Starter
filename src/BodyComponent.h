#pragma once
#include "Component.h"
#include <box2d/box2d.h>
#include <SDL.h>

// Forward declaration
class Object;
 
class BodyComponent : public Component { 
public: BodyComponent(b2WorldId world, float x, float y, float w, float h, bool isDynamic = true, float worldHeight = 1200.0f);
~BodyComponent(); 
// Position and size 
float getX() const; 
float getY() const; 
float getWidth() const { return width; } 
float getHeight() const { return height; } 
void setX(float x); 
void setY(float y); 
SDL_Rect getRect() const; 
// Velocity f
float getVx() const; 
float getVy() const; 
void setVx(float vx); 
void setVy(float vy); 
void setLinearVelocity(const b2Vec2& vel); 
b2Vec2 getLinearVelocity() const;

// Force and impulse
void applyForce(const b2Vec2& force, const b2Vec2& point);
void applyForceToCenter(const b2Vec2& force);
void applyLinearImpulse(const b2Vec2& impulse, const b2Vec2& point);
void applyLinearImpulseToCenter(const b2Vec2& impulse);

// Angular velocity
float getAngularVelocity() const;
void setAngularVelocity(float angularVelocity);

// Angle 

float getAngle() const; 
void setAngle(float angle); 
b2BodyId getBody() const { return body; } 
b2WorldId getWorld() const { return world; }

// Set userData when component is attached to object
void initializeUserData();

private:
b2WorldId world; 
b2BodyId body; 
float width; 
float height; 
float worldHeight; 
b2ShapeId shape{}; 
// Coordinate conversion SDL uses Y-down, Box2D uses Y-up 
float sdlToBox2DY(float sdlY) const { return worldHeight - sdlY; } 
float box2DToSDLY(float box2DY) const { return worldHeight - box2DY; }

};
