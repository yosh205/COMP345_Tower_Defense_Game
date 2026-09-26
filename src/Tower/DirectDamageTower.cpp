/**
 * @file DirectDamageTower.cpp
 * @brief Implements DirectDamageTower.
 */
#include "DirectDamageTower.h"

DirectDamageTower::DirectDamageTower(int row, int col)
    : Tower(row, col,
            /*buyCost*/ 100,
            /*baseRange*/ 2.5f,
            /*basePower*/ 25,
            /*baseFireRate*/ 1.2f,
            /*maxLevel*/ 3) {}

std::string DirectDamageTower::getTypeName() const { return "Direct"; }

void DirectDamageTower::fireAt(Critter& target, std::vector<Critter*>& /*allCritters*/) {
    int damage = power;
    // Extra punch against armored critters.
    if (target.getKind() == CritterKind::Armored) {
        damage = static_cast<int>(damage * 1.5f);
    }
    target.takeDamage(damage);
}
