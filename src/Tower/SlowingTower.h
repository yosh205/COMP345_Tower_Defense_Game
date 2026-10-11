/**
 * @file SlowingTower.h
 * @brief Tower that deals light damage and slows the target.
 */
#ifndef SlowingTower_h
#define SlowingTower_h

#include "Tower.h"

/**
 * @brief Support tower: each shot deals light damage and slows the target down.
 *
 * Costs 120; range 2.75 cells, power 8, 1.0 shot per second at level 1; up to level 3.
 * Higher levels slow more strongly and for longer. Fast critters are slowed 15% more.
 */
class SlowingTower : public Tower {
public:
    /**
     * @brief Creates a level-1 Slow tower.
     * @param row Row of the scenery cell it is placed on.
     * @param col Column of the scenery cell it is placed on.
     */
    SlowingTower(int row, int col);

    std::string getTypeName() const override;

    /** @return Speed multiplier applied to the target (< 1): 0.60 at level 1, 0.08 lower per level, never below 0.35. */
    float getSlowFactor() const;

    /** @return How long the slow lasts, in seconds (1.5, +0.5 per level). */
    float getSlowDuration() const;

protected:
    /**
     * @brief Deals power damage and slows the target by getSlowFactor() for getSlowDuration() seconds.
     * @param target      The critter selected by selectTarget().
     * @param allCritters All critters on the map (unused here).
     */
    void fireAt(Critter& target, std::vector<Critter*>& allCritters) override;
};

#endif // SlowingTower_h
