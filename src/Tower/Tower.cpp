/**
 * @file Tower.cpp
 * @brief Implements shared tower buy / upgrade / sell / combat loop.
 */
#include "Tower.h"

#include <algorithm>

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
    if (cooldown > 0.f) {
        cooldown -= dt;
    }
    if (cooldown > 0.f) {
        return;
    }

    const std::vector<Critter*> inRange = detectTargets(critters);
    Critter* target = selectTarget(inRange);
    if (target == nullptr) {
        return;
    }

    fireAt(*target, critters);
    cooldown = (fireRate > 0.f) ? (1.f / fireRate) : 1.f;
}
