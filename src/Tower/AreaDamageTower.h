/**
 * @file AreaDamageTower.h
 * @brief Splash tower: damages the target and nearby critters.
 */
#ifndef AreaDamageTower_h
#define AreaDamageTower_h

#include "Tower.h"

class AreaDamageTower : public Tower {
public:
    AreaDamageTower(int row, int col);

    std::string getTypeName() const override;

    /** @return Splash radius in cells around the primary target. */
    float getSplashRadius() const;

protected:
    void fireAt(Critter& target, std::vector<Critter*>& allCritters) override;
};

#endif // AreaDamageTower_h
