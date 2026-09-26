/**
 * @file SlowingTower.cpp
 * @brief Implements SlowingTower.
 */
#include "SlowingTower.h"

#include <algorithm>

SlowingTower::SlowingTower(int row, int col)
    : Tower(row, col,
            /*buyCost*/ 120,
            /*baseRange*/ 2.75f,
            /*basePower*/ 8,
            /*baseFireRate*/ 1.0f,
            /*maxLevel*/ 3) {}

std::string SlowingTower::getTypeName() const { return "Slow"; }

float SlowingTower::getSlowFactor() const {
    // Stronger slow at higher levels (lower factor).
    return std::max(0.35f, 0.60f - 0.08f * static_cast<float>(level - 1));
}

float SlowingTower::getSlowDuration() const {
    return 1.5f + 0.5f * static_cast<float>(level - 1);
}

void SlowingTower::fireAt(Critter& target, std::vector<Critter*>& /*allCritters*/) {
    target.takeDamage(power);
    float factor = getSlowFactor();
    // Fast critters feel slows a bit more.
    if (target.getKind() == CritterKind::Fast) {
        factor *= 0.85f;
    }
    target.applySlow(factor, getSlowDuration());
}
