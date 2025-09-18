cmake_minimum_required(VERSION 3.21)
project(CppSdl2Box2dTinyxml2Starter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure debug symbols are generated
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
elseif(MINGW)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g")
endif()

# vcpkg will satisfy these find_package calls via its toolchain file (set in a preset)
find_package(SDL2 CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)

add_executable(demo
    src/main.cpp
    src/Game.cpp
    src/Game.h
)

# Use pure MinGW/GCC toolchain
target_link_libraries(demo PRIVATE
    SDL2::SDL2
    box2d::box2d
    tinyxml2::tinyxml2
)

# Define SDL_MAIN_HANDLED for MinGW to avoid SDL2main issues
target_compile_definitions(demo PRIVATE SDL_MAIN_HANDLED)

# Copy assets next to the executable so relative paths "assets/..." work when you press Run
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:demo>/assets
)
add_dependencies(demo copy_assets)

# On Windows, copy dependent DLLs (e.g., SDL2.dll) next to the exe so Run works immediately
if (WIN32)
  add_custom_command(TARGET demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_RUNTIME_DLLS:demo> $<TARGET_FILE_DIR:demo>
    COMMAND_EXPAND_LISTS
  )
endif()
