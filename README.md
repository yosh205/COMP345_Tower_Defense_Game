# COMP345_Tower_Defense_Game
Assignment #1 for Concordia University course COMP 345

## Setup Instructions

### Windows option (one command)

Prerequisites (once): **CMake** and a C++ compiler — e.g. [Visual Studio 2022](https://visualstudio.microsoft.com/) with **"Desktop development with C++"**, or `winget install Kitware.CMake` plus MinGW / Build Tools.

Then from the project folder:

```powershell
.\run.ps1
```

Or double-click `run.bat`.

That configures, builds, and launches the game. The first run downloads SFML automatically (needs internet); later runs are faster.

Manual CMake (same result):

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\bin\Release\towerdefense.exe
```

(`run.ps1` still finds the exe if your generator puts it elsewhere.)

### Linux or WSL

```bash
sudo apt install libsfml-dev
g++ -std=c++17 -Wall src/main.cpp src/Map/Map.cpp src/GUI/MapView.cpp -o towerdefense -lsfml-graphics -lsfml-window -lsfml-system
./towerdefense
```

Or with CMake (also fetches SFML if needed):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/bin/towerdefense
```

### Controls

Everything is in-game menus (no console prompts):

1. **Start screen** — set rows/columns, optional fullscreen, click **Start Game**
2. **Shop** (right panel) — click Direct / Area / Slow, then click a green scenery cell to place
3. **Select tower** — click it on the map to see stats, **Upgrade** / **Sell**
4. **New map** / **Fullscreen** — sidebar buttons (**F11** also toggles fullscreen)  
   Right-click cancels shop selection

The map scales to fill the play area (left of the sidebar). Critters come in a later part.

## SFML Usage

SFML 2.6 (Simple and Fast Multimedia Library)
Used for the graphical user interface.

- **Made for 2D games.**
