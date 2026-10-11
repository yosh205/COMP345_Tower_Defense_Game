/**
 * @file AreaDamageTower.h
 * @brief Splash tower: damages the target and nearby critters.
 */
#ifndef AreaDamageTower_h
#define AreaDamageTower_h

#include "Tower.h"

/**
 * @brief Splash tower: each shot damages the target and every critter near it.
 *
 * Costs 150; range 2.0 cells, power 18, 0.8 shots per second at level 1; up to level 3.
 * The target takes full power; other critters within getSplashRadius() of it take half.
 */
class AreaDamageTower : public Tower {
public:
    /**
     * @brief Creates a level-1 Area tower.
     * @param row Row of the scenery cell it is placed on.
     * @param col Column of the scenery cell it is placed on.
     */
    AreaDamageTower(int row, int col);

    std::string getTypeName() const override;

    /** @return Splash radius in cells around the primary target (1.25, +0.25 per level). */
    float getSplashRadius() const;

protected:
    /**
     * @brief Deals full power to the target and half power (at least 1) to every
     * other living critter within the splash radius of the target.
     * @param target      The critter selected by selectTarget().
     * @param allCritters All critters on the map, checked for splash damage.
     */
    void fireAt(Critter& target, std::vector<Critter*>& allCritters) override;
};

#endif // AreaDamageTower_h
