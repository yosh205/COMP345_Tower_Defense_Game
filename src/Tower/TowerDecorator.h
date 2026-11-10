/**
 * @file TowerDecorator.h
 * @brief Assignment 3, Part 3 (Decorator pattern): changing what a tower does as it is upgraded.
 *
 * TODO(A3 Part 3): a decorator wraps a tower and is itself a tower, so the game
 * keeps a std::unique_ptr<Tower> and wraps it again on each upgrade:
 *
 *     towers[i] = std::make_unique<SplashDecorator>(std::move(towers[i]));
 *
 * Things to decide while implementing:
 * - Tower's stats (range, power, fire rate, costs, refund, level) must be virtual
 *   so a decorator can change them, and Tower::update() (detect, select, shoot)
 *   must use the outermost tower's values and fireAt().
 * - The wrapped tower keeps its cell, cooldown, facing and observers.
 * - LevelUpDecorator should replace Tower::upgrade() in the game.
 * - BurningDecorator needs a burn effect on Critter (damage repeated for a few seconds).
 * - TowerView and the sidebar should show which decorators a tower has.
 */
#ifndef TowerDecorator_h
#define TowerDecorator_h

#include <memory>

#include "Tower.h"

/**
 * @brief Base decorator: a Tower that wraps another Tower and forwards to it.
 *
 * Concrete decorators override only what they change, and call the wrapped
 * tower for everything else.
 */
class TowerDecorator : public Tower {
public:
    /**
     * @brief Wraps a tower.
     * @param inner The tower to decorate. The decorator owns it.
     */
    explicit TowerDecorator(std::unique_ptr<Tower> inner);

protected:
    std::unique_ptr<Tower> inner;  ///< The decorated tower.
};

/** @brief Raises the tower one level: next level's cost, refund, range and power change. */
class LevelUpDecorator : public TowerDecorator {
public:
    /** @param inner The tower to upgrade. */
    explicit LevelUpDecorator(std::unique_ptr<Tower> inner);
};

/** @brief Splash: the damage also hits critters near the target. */
class SplashDecorator : public TowerDecorator {
public:
    /** @param inner The tower that gets splash damage. */
    explicit SplashDecorator(std::unique_ptr<Tower> inner);
};

/** @brief Burning: the damage to the target is applied again for a few seconds after a hit. */
class BurningDecorator : public TowerDecorator {
public:
    /** @param inner The tower that gets burning damage. */
    explicit BurningDecorator(std::unique_ptr<Tower> inner);
};

/** @brief Freezing: a hit also slows the target down. */
class FreezingDecorator : public TowerDecorator {
public:
    /** @param inner The tower that gets freezing damage. */
    explicit FreezingDecorator(std::unique_ptr<Tower> inner);
};

#endif // TowerDecorator_h
