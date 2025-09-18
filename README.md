# SDL2 + Box2D + tinyxml2 Starter Project

## ⚠️ Important Notice

**This code is NOT an example of good programming practices for this class.** This project serves as a minimal demonstration to verify that you can successfully:

- Set up a C++ project with SDL2, Box2D, and tinyxml2 using vcpkg
- Build and run a basic physics simulation
- Configure your development environment (VS Code/Cursor) for C++ development

The code itself is not meant to be taken as an exlempar

## What This Project Does

This is a simple falling blocks physics simulation that demonstrates:
- SDL2 for window creation and rendering
- Box2D 3.x for 2D physics simulation
- tinyxml2 for configuration file loading
- vcpkg for dependency management

Blocks spawn randomly at the top of the screen with random colors, fall with realistic physics, and bounce off the ground.

## Prerequisites for Windows

Before cloning this project, you need to install the following on your Windows machine:

### 1. Git (Required)
- Download from: https://git-scm.com/download/win
- Used for cloning the repository and vcpkg submodule management

### 2. VS Code or Cursor (Recommended)
- VS Code: https://code.visualstudio.com/
- Cursor: https://cursor.sh/
- Install the **C/C++ Extension Pack** for IntelliSense support
- Install the **CMake Tools** and **CMake** extensions for building and configuring CMake projects

### 3. Everything Else (Auto-Installed)
The setup script will automatically install:
- **MinGW-w64** (GCC, G++, GDB) via Chocolatey
- **CMake** via Chocolatey
- **Chocolatey** (if not already installed)

## Setup Instructions

### Option 1: Automated Setup (Recommended)
1. **Clone the Repository**
   ```bash
   git clone <your-repo-url>
   cd CPSI-27703-Intro-to-Game-Programming-Fall-2025-Starter
   ```

2. **Run the Setup Script**
   ```bash
   setup.bat
   ```
   
   The script will automatically:
   - Verify all prerequisites are installed
   - **Auto-install Chocolatey (if missing)**
   - **Auto-install MinGW-w64 and CMake via Chocolatey (if missing)**
   - Initialize vcpkg submodule
   - Bootstrap vcpkg
   - Configure CMake
   - Build the project
   - Provide clear success/error messages

### Option 2: Manual Setup
If you prefer to run the commands manually:

1. **Clone the Repository**
   ```bash
   git clone <your-repo-url>
   cd CPSI-27703-Intro-to-Game-Programming-Fall-2025-Starter
   ```

2. **Initialize vcpkg Submodule**
   ```bash
   git submodule update --init --recursive
   ```

3. **Bootstrap vcpkg**
   ```bash
   cd external/vcpkg
   ./bootstrap-vcpkg.bat
   cd ../..
   ```

4. **Configure CMake**
   ```bash
   cmake --preset windows-mingw-debug
   ```

5. **Build the Project**
   ```bash
   cmake --build build/win-mingw-debug
   ```

6. **Run the Application**
   ```bash
   .\build\win-mingw-debug\demo.exe
   ```

## VS Code/Cursor Configuration

### Install Extensions
Install these extensions for the best development experience:
- **C/C++ Extension Pack** (Microsoft)
- **CMake Tools** (Microsoft)

### Ready to Use!
The project includes pre-configured VS Code settings:
- **IntelliSense** - Configured for SDL2, Box2D, and tinyxml2
- **Build Tasks** - Press `Ctrl+Shift+P` → "Tasks: Run Task" → "build-mingw-debug"
- **Debug Launch** - Press `F5` to build and run with GDB debugging

All configuration files are already included in the `.vscode/` directory.

## Project Structure

```
├── assets/
│   └── config.xml          # Configuration file
├── build/                  # Build output directory
├── external/
│   └── vcpkg/             # vcpkg package manager
├── src/
│   ├── Game.h             # Game class header
│   ├── Game.cpp           # Game class implementation
│   └── main.cpp           # Entry point
├── .vscode/               # VS Code configuration
│   ├── c_cpp_properties.json  # IntelliSense settings
│   ├── tasks.json         # Build tasks
│   └── launch.json        # Debug configuration
├── CMakeLists.txt         # CMake configuration
├── CMakePresets.json      # CMake presets
├── vcpkg.json            # Package dependencies
├── setup.bat             # Automated setup script
├── .gitignore            # Git ignore rules
└── README.md             # This file
```

## Dependencies

This project uses vcpkg to manage the following dependencies:
- **SDL2**: Cross-platform multimedia library
- **Box2D**: 2D physics engine (version 3.x)
- **tinyxml2**: XML parsing library

## Troubleshooting

### Common Issues

1. **"vcpkg not found"**
   - Make sure you've run `git submodule update --init --recursive`
   - Ensure you've bootstrapped vcpkg with `./bootstrap-vcpkg.bat`

2. **"CMake not found" or "MinGW not found"**
   - The setup script should have installed these automatically
   - Try running `setup.bat` again
   - If still failing, install manually:
     - CMake: https://cmake.org/download/
     - MinGW-w64: https://www.mingw-w64.org/downloads/
   - Restart your terminal/VS Code after installation

4. **Build errors**
   - Try cleaning: `cmake --build build/win-mingw-debug --target clean`
   - Reconfigure: `cmake --preset windows-mingw-debug`

### Getting Help

If you encounter issues:
1. Check that Git is installed and in your PATH
2. Try running `setup.bat` again (it should install everything automatically)
3. Ensure you're running commands from the project root directory
4. Check the build output for specific error messages

## Next Steps

Once you have this project building and running successfully, you're ready to:
- Start your actual game development assignments
- Use this as a reference for setting up SDL2 + Box2D projects
- Modify the code to experiment with different physics behaviors
- Add your own game features and mechanics

Remember: This starter project is just to verify your development environment works. Your actual assignments should follow proper C++ programming practices!
