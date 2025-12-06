# Box2D Physics Integration Features

This document describes the Box2D physics integration features implemented in the game engine.

## Features Implemented

### 1. Box2D Integration with userData
- All Box2D bodies store a pointer to their associated `Object` in the `userData` field
- This allows retrieval of objects during physics events (contacts, queries, raycasts)
- The userData is automatically set when a `BodyComponent` is attached to an `Object`

### 2. Dynamic Forces and Velocities
The `BodyComponent` class now supports:
- **Apply Force**: `applyForce(force, point)` and `applyForceToCenter(force)`
- **Apply Impulse**: `applyLinearImpulse(impulse, point)` and `applyLinearImpulseToCenter(impulse)`
- **Set Angular Velocity**: `setAngularVelocity(angularVelocity)` and `getAngularVelocity()`
- **Set Linear Velocity**: `setLinearVelocity(vel)` and `getLinearVelocity()`

### 3. Physics Queries

#### Raycasting
- `Engine::castRay(x1, y1, x2, y2)` - Casts a ray from point (x1, y1) to (x2, y2)
- Returns a `RaycastResult` containing:
  - `hit`: Whether the ray hit anything
  - `point`: The hit point in Box2D coordinates
  - `normal`: The surface normal at the hit point
  - `fraction`: The fraction along the ray where the hit occurred
  - `object`: Pointer to the hit object (if any)

#### AABB Querying
- `Engine::queryAABB(x, y, width, height)` - Queries all bodies within an axis-aligned bounding box
- Returns an `AABBQueryResult` containing a vector of `Object*` pointers

#### Contact Listening
- Contact events are automatically processed after each physics step
- Three types of events are logged:
  - **Begin Contact**: When two bodies start touching
  - **End Contact**: When two bodies stop touching
  - **Hit Events**: High-speed collisions (when approach speed exceeds threshold)

### 4. Runtime Body Management
- `Engine::createDynamicBody(x, y, w, h)` - Creates a dynamic body at runtime
- `Engine::createStaticBody(x, y, w, h)` - Creates a static body at runtime
- `Engine::removeObject(obj)` - Removes an object and its physics body
- `Engine::removeObjectById(id)` - Removes an object by ID

### 5. Static and Dynamic Bodies
- Bodies can be created as either static (immovable) or dynamic (affected by physics)
- Static bodies are rendered in gray
- Dynamic bodies are rendered in orange
- Contact events are automatically enabled for dynamic bodies

## Interactive Controls

The following keyboard controls are available for testing physics features:

| Key | Action |
|-----|--------|
| **F** | Apply force to player (pushes right) |
| **G** | Apply impulse to player (jump impulse) |
| **R** | Set angular velocity on player (rotation) |
| **T** | Cast ray forward from player position |
| **Q** | Perform AABB query around player |
| **1** | Spawn a dynamic body at camera center |
| **2** | Spawn a static body at camera center |
| **X** | Delete last spawned object (non-player, non-ground) |

## Visual Debugging

- **Raycasts**: Yellow lines with start/end points. Red dot indicates hit point, green dot indicates no hit.
- **AABB Queries**: Cyan outlined rectangles showing the query area.
- **Contact Events**: Logged to console with object IDs.

## Coordinate System Notes

- SDL uses Y-down coordinates (Y increases downward)
- Box2D uses Y-up coordinates (Y increases upward)
- All coordinate conversions are handled automatically in `BodyComponent`
- Raycast and AABB query results are in Box2D coordinates but can be converted using `box2DToSDLY()`

## Example Usage

```cpp
// Cast a ray
Engine::RaycastResult result = engine.castRay(100, 100, 500, 300);
if (result.hit) {
    std::cout << "Hit object: " << result.object->getId() << std::endl;
}

// Query AABB
Engine::AABBQueryResult query = engine.queryAABB(200, 200, 400, 400);
for (Object* obj : query.objects) {
    std::cout << "Found object: " << obj->getId() << std::endl;
}

// Apply force to a body
BodyComponent* body = object->getComponent<BodyComponent>();
body->applyForceToCenter(b2Vec2{100.0f, 0.0f});

// Create a dynamic body at runtime
Object* newObj = engine.createDynamicBody(400, 300, 50, 50);
```

