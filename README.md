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

The map scales to fill the play area (left of the sidebar).

## Game Rules

The game follows the usual rules of the tower defense genre: enemies ("critters")
walk along a fixed path from an entry to an exit, and the player places towers
beside the path to destroy them before they get through. Towers are bought and
upgraded with money earned by defeating enemies, and enemies arrive in waves
that become harder over time [1][2]. The specific rules below come from the
COMP 345 Assignment 1 specification [3]; the numbers are the values used in the code.

### Map
- The map is a grid whose number of rows and columns is chosen before it is created
  (4–24 rows and 4–30 columns in the start menu). A new map is blank: every cell is scenery.
- **Scenery** cells: towers can be placed here; critters cannot walk here.
- **Path** cells: critters walk here; towers cannot be placed here.
- There is **exactly one entry** and **exactly one exit**, both path cells.
- Path cells must connect the entry to the exit, moving up, down, left or right
  (not diagonally). `Map::isValid()` checks all of these rules.

### Towers
- Every tower has a **buying cost**, **range**, **power**, **rate of fire**, **level**
  (1 to 3) and **refund value**.
- **Upgrading** costs `buy cost + level × buy cost / 2` and increases power by 25%,
  range by 15% and rate of fire by 10% per level.
- **Selling** refunds 70% of all the money spent on the tower (purchase plus upgrades),
  so a higher-level tower sells for more.
- Each frame a tower **detects** the critters within its range, **selects** the one
  furthest along the path (closest to the exit), and **shoots** it when its reload time is over.

| Tower | Cost | Range | Power | Shots/s | Effect |
|---|---|---|---|---|---|
| Direct | 100 | 2.5 | 25 | 1.2 | Single target; +50% damage against Armored critters |
| Area | 150 | 2.0 | 18 | 0.8 | Hits the target and every critter within 1.25 cells (+0.25 per level); splash victims take half damage |
| Slow | 120 | 2.75 | 8 | 1.0 | Light damage and slows the target to 60% speed (stronger per level) for 1.5 s (+0.5 s per level); Fast critters are slowed 15% more |

### Critters and waves
- Every critter has **hit points**, **speed**, **level**, **reward** and **strength**,
  and is one of three kinds: Normal, Armored (more hit points, slower) or Fast (fewer hit points, faster).
- The **critter group generator** is called when the player clicks **Start Wave**.
  Each wave has 10 Normal, 5 Armored and 5 Fast critters; their level equals the
  wave number, and their hit points (+10% per wave) and speed (+0.1 cells/s per wave)
  grow with it, so **every wave is harder than the one before**.
- Critters enter one after another at the entry, one per second, and follow the
  shortest path to the exit (`Map::findPath()`). They never move backwards.
- Towers reduce a critter's hit points; it **dies at 0**.
- **Killing** a critter pays a reward **proportional to its level**: 5 coins per level
  (+50% for Armored critters).
- A critter that **reaches the exit steals coins** according to its **strength**:
  3 coins per point of strength (strength = level, +1 for Fast critters).
  In games such as Bloons TD an escaping enemy costs lives instead [2];
  this game uses coin theft, as the assignment requires [3].
- The player starts with 500 coins; gold never goes below 0.

### Sources
1. "Tower defense", *Wikipedia*. https://en.wikipedia.org/wiki/Tower_defense
2. "Bloons TD", *Wikipedia*. https://en.wikipedia.org/wiki/Bloons_TD
3. COMP 345 Fall 2026, *Assignment #1* specification (Parts 1–3), Concordia University.

## Design

The game logic is kept separate from the graphics so each part can be tested
without a window (see `tests/tests.cpp`).

| Folder | Classes | Responsibility |
|---|---|---|
| `src/Map` | `Map`, `generateRandomMap()` | Grid of cells, setters, validity check, entry-to-exit path (breadth-first search) |
| `src/Tower` | `Tower` (abstract), `DirectDamageTower`, `AreaDamageTower`, `SlowingTower` | Costs, levels, upgrade/sell, detect → select → shoot. Subclasses only change the shot's effect (`fireAt()`) |
| `src/Critter` | `Critter`, `CritterGroupGenerator` | Critter stats and effects; creating a wave, spawning, moving along the path, rewards and theft |
| `src/GUI` | `MapView`, `TowerView`, `CritterView`, `Hud` | Drawing with SFML and the menus/sidebar. Views only read the game objects |
| `src/main.cpp` | — | Driver: start menu, then each frame spawn → move → pay coins → towers shoot → draw |

```
                 Tower (abstract)
                /       |        \
   DirectDamageTower  AreaDamageTower  SlowingTower

   Map ──findPath()──► CritterGroupGenerator ──owns──► Critter
                                 ▲                      ▲
   main.cpp (game loop) ─────────┴── Tower::update() ───┘ (damage / slow)
```

## SFML Usage

SFML 2.6 (Simple and Fast Multimedia Library)
Used for the graphical user interface.

- **Made for 2D games.**
