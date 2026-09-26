/**
 * @file AreaDamageTower.cpp
 * @brief Implements AreaDamageTower.
 */
#include "AreaDamageTower.h"

#include <algorithm>

AreaDamageTower::AreaDamageTower(int row, int col)
    : Tower(row, col,
            /*buyCost*/ 150,
            /*baseRange*/ 2.0f,
            /*basePower*/ 18,
            /*baseFireRate*/ 0.8f,
            /*maxLevel*/ 3) {}

std::string AreaDamageTower::getTypeName() const { return "Area"; }

float AreaDamageTower::getSplashRadius() const {
    return 1.25f + 0.25f * static_cast<float>(level - 1);
}

void AreaDamageTower::fireAt(Critter& target, std::vector<Critter*>& allCritters) {
    const float splash = getSplashRadius();
    const int tr = target.getRow();
    const int tc = target.getCol();

    for (Critter* c : allCritters) {
        if (c == nullptr || !c->isAlive()) {
            continue;
        }
        if (c->distanceTo(tr, tc) <= splash) {
            // Full power on the primary target; half on splash victims.
            const int dmg = (c == &target) ? power : std::max(1, power / 2);
            c->takeDamage(dmg);
        }
    }
}
