# Component-Based Game Engine Demo

This project is a small 2D game demo built using a **component-based architecture** in C++ with SDL2. The assignment focuses on modularizing core game functionalities into reusable and interchangeable components.

---

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Components](#components)
- [Controls](#controls)
- [Demo](#demo)
- [Level XML Example](#level-xml-example)
- [Project Structure](#project-structure)
- [Notes](#notes)
- [Building & Running](#building--running)

---

## Overview

The engine demonstrates a **component-based design** where each `GameObject` is composed of modular components. This allows flexible object creation without needing multiple subclasses for different object types.

Components handle specific responsibilities like rendering, movement, input, or behaviors. The engine supports loading game objects and components from XML for quick scene setup.

---

## Features

- **Component System**: Add multiple components to a single `GameObject`.
- **Sprite Rendering**: Display colored rectangles or PNG textures.
- **Player Movement**: Controlled via `W`, `A`, `S`, `D`.
- **Behavior Components**: Encapsulate logic like automated enemy movement.
- **XML Level Loading**: Define objects and components via XML for easy modification.

> ⚠️ **Collision Handling**: Not implemented yet. Objects currently can pass through each other.

---

## Components

| Component | Description |
|-----------|-------------|
| `Component` | Abstract base class for all components. Provides `update()` and `draw()` methods. |
| `SpriteComponent` | Handles rendering of textures or colored rectangles. |
| `BodyComponent` | Manages object physics (position, velocity, angle). |
| `ControllerComponent` | Handles player input for movement. |
| `Behavior1Component` | Example behavior for automated movement (e.g., enemy patrol). |
| `Behavior2Component` | Planned for future use (e.g., walls or interactive objects). |

---

## Controls

- `W` → Move Up  
- `A` → Move Left  
- `S` → Move Down  
- `D` → Move Right  

The player can move in all four directions. Diagonal movement is possible by pressing two keys simultaneously.

---

## Demo

The demo scene contains:

1. **Player** – green rectangle or texture (`assets/player.png`).
2. **Enemy** – red rectangle that moves automatically using `Behavior1Component`.
3. **Crate** – brown rectangle (static object).

> Currently, objects do not collide; the crate will not stop the player or enemy yet.

![Demo Screenshot](assets/demo_screenshot.png)

---

## Level XML Example

```xml
<?xml version="1.0"?>
<Level>
    <GameObject id="player">
        <BodyComponent x="2" y="3" angle="0" />
        <SpriteComponent r="0" g="255" b="0" image="assets/player.png"/>
        <ControllerComponent speed="3" />
    </GameObject>

    <GameObject id="enemy">
        <BodyComponent x="6" y="3" angle="0" />
        <SpriteComponent r="255" g="0" b="0" />
        <Behavior1Component speed="1.5" minX="2" maxX="6" />
    </GameObject>

    <GameObject id="crate">
        <BodyComponent x="4" y="3" angle="0" />
        <SpriteComponent r="139" g="69" b="19" />
    </GameObject>
</Level>


Project Structure

/assets
    player.png
    demo_screenshot.png
    level.xml
/src
    main.cpp
    Engine.h/cpp
    GameObject.h/cpp
    Component.h/cpp
    SpriteComponent.h/cpp
    BodyComponent.h/cpp
    ControllerComponent.h/cpp
    Behavior1Component.h/cpp
    ComponentFactory.h/cpp
    Input.h/cpp
README.md


Notes

Uses SDL2 and SDL2_image for graphics.

Game objects and components are loaded from XML.

Collision detection is planned for future implementation.

The architecture allows easy addition of new components and behaviors.


Building & Running

Ensure SDL2 and SDL2_image are installed on your system.

Compile the project. Example using g++:

g++ src/*.cpp -o GameDemo -lSDL2 -lSDL2_image
