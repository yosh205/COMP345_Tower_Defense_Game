/**
 * @file Tower.cpp
 * @brief Implements shared tower buy / upgrade / sell / combat loop.
 */
#include "Tower.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr float kRefundRatio = 0.70f; // 70% of money spent when sold
}

Tower::Tower(int row, int col, int buyCost, float baseRange, int basePower,
             float baseFireRate, int maxLevel)
    : row(row),
      col(col),
      level(1),
      maxLevel(std::max(1, maxLevel)),
      buyCost(buyCost),
      totalSpent(buyCost),
      baseRange(baseRange),
      basePower(basePower),
      baseFireRate(baseFireRate),
      range(0.f),
      power(0),
      fireRate(0.f),
      cooldown(0.f) {
    refreshStats();
}

int Tower::getRow() const { return row; }
int Tower::getCol() const { return col; }
int Tower::getLevel() const { return level; }
int Tower::getMaxLevel() const { return maxLevel; }
int Tower::getBuyCost() const { return buyCost; }

int Tower::getUpgradeCost() const {
    if (!canUpgrade()) {
        return 0;
    }
    // Each next level costs more than the last.
    return buyCost + level * (buyCost / 2);
}

int Tower::getRefundValue() const {
    return static_cast<int>(totalSpent * kRefundRatio);
}

float Tower::getRange() const { return range; }
int Tower::getPower() const { return power; }
float Tower::getFireRate() const { return fireRate; }
float Tower::getFacingDegrees() const { return facingDegrees; }
bool Tower::hasJustFired() const { return justFired; }
float Tower:: getLastTargetRow() const { return lastTargetRow; }
float Tower:: getLastTargetCol() const { return lastTargetCol; }

bool Tower::canUpgrade() const { return level < maxLevel; }

bool Tower::upgrade() {
    if (!canUpgrade()) {
        return false;
    }
    totalSpent += getUpgradeCost();
    ++level;
    refreshStats();
    return true;
}

void Tower::refreshStats() {
    // Gradual capacity increase per level.
    const float levelBonus = 1.f + 0.25f * static_cast<float>(level - 1);
    range = baseRange * (1.f + 0.15f * static_cast<float>(level - 1));
    power = static_cast<int>(basePower * levelBonus);
    fireRate = baseFireRate * (1.f + 0.10f * static_cast<float>(level - 1));
}

std::vector<Critter*> Tower::detectTargets(const std::vector<Critter*>& critters) const {
    std::vector<Critter*> inRange;
    for (Critter* c : critters) {
        if (c != nullptr && c->isAlive() && c->distanceTo(row, col) <= range) {
            inRange.push_back(c);
        }
    }
    return inRange;
}

Critter* Tower::selectTarget(const std::vector<Critter*>& inRange) const {
    Critter* best = nullptr;
    for (Critter* c : inRange) {
        if (c == nullptr || !c->isAlive()) {
            continue;
        }
        if (best == nullptr || c->getPathIndex() > best->getPathIndex()) {
            best = c;
        }
    }
    return best;
}

void Tower::update(float dt, std::vector<Critter*>& critters) {
    justFired = false; // only true on the frame the towers actually shoot
    if (cooldown > 0.f) {
        cooldown -= dt;
    }
    
    // Detect and select every frame (not only when ready to fire) so the
    // tower keeps turning to follow its target while it reloads.
    const std::vector<Critter*> inRange = detectTargets(critters);
    Critter* target = selectTarget(inRange);
    if (target == nullptr) { // if there is no target, stop
        return;
    }

    // Face the target. Screen y grows downward (+row), so atan2(row, col)
    // gives SFML's clockwise angle with 0 degrees pointing right.
    const float dRow = target->getRow() - static_cast<float>(row);
    const float dCol = target->getCol() - static_cast<float>(col);
    facingDegrees = std::atan2(dRow, dCol) * 180.f / 3.14159265f;

    if (cooldown > 0.f) {
        return; // still reloading
    }

    // Remember where the target is for the projectile, then shoot.
    lastTargetRow = target->getRow();
    lastTargetCol = target->getCol();
    fireAt(*target, critters);
    justFired = true;
    cooldown = (fireRate > 0.f) ? (1.f / fireRate) : 1.f;
}
