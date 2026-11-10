/**
 * @file TargetingStrategy.h
 * @brief Assignment 3, Part 1 (Strategy pattern): how a tower chooses which critter to shoot.
 *
 * TODO(A3 Part 1): implement the four strategies in TargetingStrategy.cpp, give
 * Tower a std::unique_ptr<TargetingStrategy> with setStrategy() / getStrategy(),
 * and make Tower::selectTarget() ask the strategy. Default: NearestToExitStrategy
 * (what Tower::selectTarget() does today). Add tests and a way to change the
 * strategy of a selected tower in the game.
 */
#ifndef TargetingStrategy_h
#define TargetingStrategy_h

#include <string>
#include <vector>

class Critter;
class Tower;

/**
 * @brief Strategy interface: picks one critter among those in a tower's range.
 *
 * The tower first detects the living critters within its range, then asks its
 * strategy to choose one. Changing the strategy changes which critter is shot,
 * without changing the tower class.
 */
class TargetingStrategy {
public:
    /** @brief Virtual so strategies can be deleted through a TargetingStrategy pointer. */
    virtual ~TargetingStrategy() = default;

    /**
     * @brief Chooses the critter to shoot.
     * @param tower   The tower that is about to fire (for its position).
     * @param inRange Living critters within the tower's range (may be empty).
     * @return The critter to shoot, or nullptr if inRange is empty.
     */
    virtual Critter* choose(const Tower& tower, const std::vector<Critter*>& inRange) const = 0;

    /** @return Short name for the sidebar, e.g. "NEAREST EXIT" (bitmap-font characters only). */
    virtual std::string getName() const = 0;
};

/** @brief Shoots the critter closest to the tower. */
class NearestToTowerStrategy : public TargetingStrategy {
public:
    Critter* choose(const Tower& tower, const std::vector<Critter*>& inRange) const override;
    std::string getName() const override;
};

/** @brief Shoots the critter furthest along the path, i.e. closest to the exit. */
class NearestToExitStrategy : public TargetingStrategy {
public:
    Critter* choose(const Tower& tower, const std::vector<Critter*>& inRange) const override;
    std::string getName() const override;
};

/** @brief Shoots the critter with the most hit points left. */
class StrongestStrategy : public TargetingStrategy {
public:
    Critter* choose(const Tower& tower, const std::vector<Critter*>& inRange) const override;
    std::string getName() const override;
};

/** @brief Shoots the critter with the fewest hit points left. */
class WeakestStrategy : public TargetingStrategy {
public:
    Critter* choose(const Tower& tower, const std::vector<Critter*>& inRange) const override;
    std::string getName() const override;
};

#endif // TargetingStrategy_h
