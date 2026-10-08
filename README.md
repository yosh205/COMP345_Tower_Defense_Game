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

### Linux or WSL (Ubuntu)

**Option 1 — g++ with the system SFML** (quickest):

```bash
sudo apt install g++ libsfml-dev
g++ -std=c++17 -Wall -Isrc $(find src -name '*.cpp') -o towerdefense -lsfml-graphics -lsfml-window -lsfml-system
./towerdefense
```

`$(find src -name '*.cpp')` compiles every source file in `src/`, so the command
still works when files are added. Run it from the project folder.

**Option 2 — CMake** (same build as Windows; downloads and compiles SFML 2.6.1 the first time):

```bash
sudo apt install cmake g++ git libudev-dev libx11-dev libxrandr-dev libxcursor-dev libxi-dev libgl-dev libfreetype-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/bin/towerdefense
```

The `apt` packages are needed to compile SFML from source. The first build takes a
few minutes; after a code change only `cmake --build build -j` is needed.

### Running the tests

`tests/tests.cpp` checks the game rules of the map, towers, critters and waves
(83 checks). It is a separate program and does not need SFML or a screen.

```bash
# With CMake (after building as above)
./build/bin/tests                        # Windows: .\build\bin\Release\tests.exe
ctest --test-dir build --output-on-failure

# Or with g++ only
g++ -std=c++17 -Wall -Isrc tests/tests.cpp src/Map/Map.cpp src/Critter/*.cpp src/Tower/*.cpp -o tests_run
./tests_run
```

Every check prints `PASS` or `FAIL`; the last line shows how many passed.

### Documentation

The code is documented with Doxygen; this README is the main page.

```bash
doxygen docs/Doxyfile        # run from the project folder
```

Then open `docs/html/index.html` in a browser.

### Controls

Everything is in-game menus (no console prompts):

1. **Start screen** — set rows/columns, optional fullscreen, click **Start Game**
2. **Shop** (right panel) — click Direct / Area / Slow, then click a green scenery cell to place
3. **Select tower** — click it on the map to see its range and stats, **Upgrade** / **Sell**
4. **Start Wave** — sidebar button; critters enter one per second. While a wave runs the
   sidebar shows how many critters are left; when it ends, the button returns for the next, harder wave.
5. **New map** / **Fullscreen** — sidebar buttons (**F11** also toggles fullscreen)  
   Right-click cancels shop selection

The sidebar shows your **gold** (it rises when towers kill critters and drops when
critters reach the exit) and the current **wave**. Towers turn to face their target.
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

## Tools and Libraries

### SFML 2.6 (Simple and Fast Multimedia Library) — graphical user interface

**Why SFML:**
- **Made for 2D games.** The game only needs coloured grid cells, simple shapes for towers
  and critters, mouse clicks and a frame loop. SFML provides these directly
  (`sf::RectangleShape`, `sf::CircleShape`, `sf::RenderWindow`, events).
- **Object-oriented C++ API**, which fits the course's focus on C++ classes.
  Lower-level C libraries such as SDL2 need more setup code for the same drawing.
- **Cross-platform.** The same code builds on Windows, Linux/WSL and macOS, so every
  team member and the lab computers can build it.
- **Lighter than a GUI toolkit such as Qt**, which is designed for forms and widgets
  rather than real-time games and is much larger to install.

**How it is used:**
- SFML is only used in `src/GUI/` and `src/main.cpp`. `Map`, `Tower` and `Critter` contain
  no SFML code: the views (`MapView`, `TowerView`, `CritterView`) read the game objects
  and draw them. This keeps the game logic independent of the display and testable
  without a window.
- Text is drawn with a small built-in bitmap font (`SimpleText`) instead of SFML fonts,
  which avoids depending on font files and FreeType linking problems on Windows/MinGW.
- We use version **2.6**, not 3.x: SFML 3 changed the event and drawing API, and 2.6 is
  the version packaged by Ubuntu 24.04 (`libsfml-dev`).

### CMake — build system
One build description for Windows (Visual Studio or MinGW) and Linux. It downloads
SFML 2.6.1 automatically (`FetchContent`) and links it statically, so no DLLs need to
be copied next to the program on Windows. It also builds the test program.

### Doxygen — documentation
Generates the documentation from the comments in the code, as required by the
assignment (`docs/Doxyfile`; see *Documentation* above).
