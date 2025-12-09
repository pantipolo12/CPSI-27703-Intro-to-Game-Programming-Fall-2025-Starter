cmake_minimum_required(VERSION 3.21)
project(CppSdl2Box2dTinyxml2Starter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(
    demo 
    src/main.cpp 
    src/Object.h
    src/Object.cpp 
    src/Engine.h
    src/Engine.cpp 
    src/ImageDevice.h
    src/ImageDevice.cpp
    src/InputDevice.h
    src/InputDevice.cpp
    src/BodyComponent.h
    src/BodyComponent.cpp
    src/SpriteComponent.h
    src/SpriteComponent.cpp
    src/Component.h
    src/Component.cpp
    src/MissileComponent.h
    src/MissileComponent.cpp
    src/BounceComponent.h
    src/BounceComponent.cpp
    src/PhysicsComponent.h
    src/PhysicsComponent.cpp
    src/CharacterComponent.h
    src/CharacterComponent.cpp
    src/View.h
    src/ControlerComponent.h
    src/KeyComponent.h
    src/KeyComponent.cpp
    src/DoorComponent.h
    src/DoorComponent.cpp
    src/HealthComponent.h
    src/HealthComponent.cpp
    src/Timer.h
    src/LevelLoader.h
    src/LevelLoader.cpp
    src/SaveGame.h
    src/SaveGame.cpp
    src/SolidComponent.h
    src/AnimateComponent.h
    src/AnimateComponent.cpp
    src/Menu.h
    src/Menu.cpp
)

# Link libraries
target_link_libraries(demo PRIVATE
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    box2d::box2d
    tinyxml2::tinyxml2
    yaml-cpp::yaml-cpp
    nlohmann_json::nlohmann_json
)

# Define SDL_MAIN_HANDLED for MinGW
target_compile_definitions(demo PRIVATE SDL_MAIN_HANDLED)

# Copy assets and DLLs
add_custom_command(TARGET demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:demo>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:demo> $<TARGET_FILE_DIR:demo>
    COMMAND_EXPAND_LISTS
)
