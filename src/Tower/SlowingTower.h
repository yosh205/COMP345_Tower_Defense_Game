/**
 * @file SlowingTower.h
 * @brief Tower that deals light damage and slows the target.
 */
#ifndef SlowingTower_h
#define SlowingTower_h

#include "Tower.h"

class SlowingTower : public Tower {
public:
    SlowingTower(int row, int col);

    std::string getTypeName() const override;

    /** @return Speed multiplier applied to the target (< 1). */
    float getSlowFactor() const;

    /** @return How long the slow lasts, in seconds. */
    float getSlowDuration() const;

protected:
    void fireAt(Critter& target, std::vector<Critter*>& allCritters) override;
};

#endif // SlowingTower_h
