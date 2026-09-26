/**
 * @file Critter.cpp
 * @brief Implements Critter. Function docs are in Critter.h.
 */
#include "Critter.h"

#include <algorithm>
#include <cmath>

Critter::Critter(CritterKind kind, int maxHitPoints, float baseSpeed, int row, int col)
    : kind(kind),
      maxHitPoints(maxHitPoints),
      hitPoints(maxHitPoints),
      baseSpeed(baseSpeed),
      slowFactor(1.f),
      slowTimeLeft(0.f),
      row(row),
      col(col),
      pathIndex(0) {}

CritterKind Critter::getKind() const { return kind; }
int Critter::getHitPoints() const { return hitPoints; }
int Critter::getMaxHitPoints() const { return maxHitPoints; }
bool Critter::isAlive() const { return hitPoints > 0; }

float Critter::getSpeed() const { return baseSpeed * slowFactor; }
float Critter::getBaseSpeed() const { return baseSpeed; }

int Critter::getRow() const { return row; }
int Critter::getCol() const { return col; }

void Critter::setPosition(int row, int col) {
    this->row = row;
    this->col = col;
}

int Critter::getPathIndex() const { return pathIndex; }
void Critter::setPathIndex(int index) { pathIndex = index; }

void Critter::takeDamage(int amount) {
    if (amount <= 0) {
        return;
    }
    hitPoints = std::max(0, hitPoints - amount);
}

void Critter::applySlow(float factor, float durationSeconds) {
    if (factor <= 0.f || factor >= 1.f || durationSeconds <= 0.f) {
        return;
    }
    // Keep the stronger slow; always refresh duration when reapplied.
    if (factor < slowFactor) {
        slowFactor = factor;
    }
    slowTimeLeft = durationSeconds;
}

void Critter::updateEffects(float dt) {
    if (slowTimeLeft <= 0.f) {
        slowFactor = 1.f;
        return;
    }
    slowTimeLeft -= dt;
    if (slowTimeLeft <= 0.f) {
        slowTimeLeft = 0.f;
        slowFactor = 1.f;
    }
}

float Critter::distanceTo(int row, int col) const {
    const float dr = static_cast<float>(this->row - row);
    const float dc = static_cast<float>(this->col - col);
    return std::sqrt(dr * dr + dc * dc);
}
