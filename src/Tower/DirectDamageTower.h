/**
 * @file DirectDamageTower.h
 * @brief Single-target tower; bonus damage vs armored critters.
 */
#ifndef DirectDamageTower_h
#define DirectDamageTower_h

#include "Tower.h"

class DirectDamageTower : public Tower {
public:
    DirectDamageTower(int row, int col);

    std::string getTypeName() const override;

protected:
    void fireAt(Critter& target, std::vector<Critter*>& allCritters) override;
};

#endif // DirectDamageTower_h
