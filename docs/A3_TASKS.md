# Assignment 3 — task list

**Due: Friday, November 13, 2026 at 17:00** on EAS, category "programming assignment 3". No late submissions.
One zip, with a README that has build instructions, the game rules (with sources) and the design.

Every new file below already exists as a skeleton: its header says what the class must do,
and each `.cpp` has a `TODO(A3 Part N)` comment. The skeletons build, so work on your own
branch from `main` and open a pull request when your part works.

Suggested owners — change them in this file if the team agrees on something else.

## Part 1 — Tower Strategy pattern (owner: Joshua)
Choose which critter in range a tower shoots.
- `src/Tower/TargetingStrategy.h/.cpp` — `TargetingStrategy` interface and four strategies:
  nearest to the tower, nearest to the exit (today's behaviour), strongest, weakest.
- `src/Tower/Tower.h/.cpp` — the tower holds a strategy (`setStrategy()`), `selectTarget()` asks it.
- `src/GUI/TowerStrategyDriver.h/.cpp` — demo window.
- Game: a button on the selected tower that changes its strategy.
- Tests: each strategy picks the expected critter; changing the strategy changes the target.

## Part 2 — Critter Factory pattern (owner: Jose)
Waves get harder as more waves are destroyed.
- `src/Critter/CritterFactory.h/.cpp` (already started):
  - add an include guard and Doxygen comments;
  - speeds should grow with the wave again (Fast is a fixed 3.5 and Armored a fixed 0.2 right now);
  - the factory should decide each wave: how many critters of each kind for wave *n*
    (today `CritterGroupGenerator` always asks for 10 Normal, 5 Armored, 5 Fast).
- `src/GUI/CritterFactoryDriver.h/.cpp` — demo window.
- Tests: a later wave is harder (more hit points / faster / more critters) than an earlier one.

## Part 3 — Tower Decorator pattern (owner: Varak)
Upgrades change what a tower does.
- `src/Tower/TowerDecorator.h/.cpp` — `TowerDecorator` and `LevelUpDecorator`,
  `SplashDecorator`, `BurningDecorator`, `FreezingDecorator`. Read the TODO at the top of the header first.
- `src/Tower/Tower.h/.cpp` — stats become virtual so decorators can change them.
- `src/Critter/Critter.h/.cpp` — a burn effect for `BurningDecorator`.
- `src/GUI/TowerDecoratorDriver.h/.cpp` — demo window.
- Game: Upgrade wraps the tower in `LevelUpDecorator`; buttons add splash / burning / freezing.
- Tests: each decorator changes the stats or the shot as expected, and they stack.

## Shared (owner: John + everyone)
- Merge the critter inspection work from `map-select` (add `#include <cmath>` to `CritterView.cpp`).
- Tests for all three parts in `tests/tests.cpp`.
- README: an Assignment 3 section — game rules with sources, the design of each pattern, how to run the drivers.
- Friday morning: merge everything, build with CMake and with the README's g++ commands, run the tests, zip, submit.
