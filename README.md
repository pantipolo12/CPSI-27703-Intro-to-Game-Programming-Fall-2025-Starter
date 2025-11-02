# Component-Based Game Engine Demo

## Overview
This project is a simple 2D game engine built using a **component-based architecture**. The goal of the assignment is to modularize core game functionalities into interchangeable components, allowing for flexibility, reusability, and easy expansion. The engine is implemented in C++ using **SDL2** for rendering and input handling.

The playable demo showcases the following features:

- Player movement and control
- Enemy behaviors (patrolling)
- Rendering via `SpriteComponent`
- Component-based architecture allowing for modular game object behavior

---

## Architecture

### GameObject
- Acts as a container for components.
- Stores components in a `std::vector<std::unique_ptr<Component>>`.
- Methods:
  - `add<T>()` – Adds a component of type `T`.
  - `get<T>()` – Retrieves a pointer to a component of type `T`.
  - `update(dt, input)` – Calls `update` on all components.
  - `draw(renderer)` – Calls `draw` on all components that implement it.

### Component Hierarchy
- **Abstract base class `Component`**
  - `virtual void update(float dt, const Input& input) = 0`
  - `virtual void draw(SDL_Renderer* renderer) = 0`
  - `GameObject& parent()` – Returns the parent object
- **Implemented Components**
  - `SpriteComponent` – Handles graphics for objects, including textures or colored rectangles.
  - `BodyComponent` – Stores position, velocity, and rotation for objects.
  - `ControllerComponent` – Handles player input and movement logic.
  - `Behavior1Component` – Example enemy AI movement.

---

## Component Factory
- Responsible for creating game objects and their components from **XML files**.
- Uses a `map<string, function>` to associate XML tags with component creation logic.
- Reads component attributes such as position, size, and textures.
- Example XML structure:

```xml
<GameObject id="player">
    <BodyComponent x="2" y="3" angle="0" />
    <SpriteComponent r="0" g="255" b="0" image="assets/player.png"/>
    <ControllerComponent speed="3" />
</GameObject>

<GameObject id="enemy">
    <BodyComponent x="6" y="3" angle="0" />
    <SpriteComponent r="255" g="0" b="0"/>
    <Behavior1Component speed="1.5" minX="2" maxX="6" />
</GameObject>
