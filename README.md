# Component-Based Game Engine Demo

A **2D game engine demo** built in C++ using **SDL2**, designed to showcase a **component-based architecture**. Each `GameObject` is composed of modular, reusable components, allowing flexible creation of objects without relying on deep inheritance hierarchies.

This project serves as both a **learning tool** for game engine design and a **demo environment** for experimenting with behaviors, rendering, and input handling.

---

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Architecture & Components](#architecture--components)
- [Controls](#controls)
- [Demo](#demo)
- [Level XML Example](#level-xml-example)
- [Project Structure](#project-structure)
- [Notes](#notes)
- [Building & Running](#building--running)

---

## Overview

The engine uses a **component-based design**, meaning:

- A `GameObject` is **just a container** for components.
- Components encapsulate **specific responsibilities** such as rendering, physics, input, or behaviors.
- New behaviors or object types can be added without changing core classes.

Scenes can be defined entirely in **XML**, making level design fast and modular.

---

## Features

- **Component System** – Attach multiple components to a single `GameObject`.  
- **Rendering** – Display textures or simple colored rectangles.  
- **Input Handling** – Player movement with `W`, `A`, `S`, `D`.  
- **Behavior Components** – Automated logic (e.g., enemy patrols).  
- **XML Level Loading** – Load and configure objects via XML files.  
- **Extensible Architecture** – Easily add new components and behaviors.  

>**Note:** Collision detection is not implemented yet. Objects can currently pass through each other.

---

## Architecture & Components

| Component | Description |
|-----------|-------------|
| `Component` | Abstract base class. Provides `update()` and `draw()` methods. |
| `SpriteComponent` | Handles rendering textures or colored rectangles. |
| `BodyComponent` | Manages position, velocity, and rotation. |
| `ControllerComponent` | Processes player input for movement. |
| `Behavior1Component` | Example automated behavior (e.g., enemy patrol). |
| `Behavior2Component` | Placeholder for future behaviors (walls, triggers, etc.). |

This modular approach allows **mix-and-match functionality**, reducing the need for subclassing every object type.

---

## Controls

- `W` - n/a  
- `A` - Move Left  
- `S` - n/a  
- `D` - Move Right  
- `Space` - Jump 
> Diagonal movement is possible by pressing two keys simultaneously.

---

## Demo

The demo scene includes:

1. **Player** – Green rectangle or texture (`assets/player.png`) controlled by the player.  
2. **Enemy** – Red rectangle with automated movement (`Behavior1Component`).  
3. **Crate** – Brown rectangle, static object.

> Objects currently do not collide. The player and enemy can pass through crates.

![Demo Screenshot](assets/myGameScreenShot.jpg)

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
