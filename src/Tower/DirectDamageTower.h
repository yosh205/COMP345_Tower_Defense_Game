/**
 * @file DirectDamageTower.h
 * @brief Single-target tower; bonus damage vs armored critters.
 */
#ifndef DirectDamageTower_h
#define DirectDamageTower_h

#include "Tower.h"

/**
 * @brief Single-target tower: each shot damages one critter.
 *
 * Costs 100; range 2.5 cells, power 25, 1.2 shots per second at level 1; up to level 3.
 * Deals 50% extra damage to Armored critters.
 */
class DirectDamageTower : public Tower {
public:
    /**
     * @brief Creates a level-1 Direct tower.
     * @param row Row of the scenery cell it is placed on.
     * @param col Column of the scenery cell it is placed on.
     */
    DirectDamageTower(int row, int col);

    std::string getTypeName() const override;

protected:
    /**
     * @brief Deals power damage to the target (+50% if it is Armored).
     * @param target      The critter selected by selectTarget().
     * @param allCritters All critters on the map (unused here).
     */
    void fireAt(Critter& target, std::vector<Critter*>& allCritters) override;
};

#endif // DirectDamageTower_h
