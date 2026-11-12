/**
 * @file TargetingStrategy.cpp
 * @brief Implements the targeting strategies. Function docs are in TargetingStrategy.h.
 */
#include "TargetingStrategy.h"
#include "Tower.h"

// TODO(A3 Part 1): implement choose() and getName() for
//   NearestToTowerStrategy  (smallest Critter::distanceTo(tower row, tower col))
//   NearestToExitStrategy   (largest Critter::getProgress(); same as Tower::selectTarget() today)
//   StrongestStrategy       (most Critter::getHitPoints())
//   WeakestStrategy         (fewest Critter::getHitPoints())
// Each returns nullptr when inRange is empty. Break ties the same way every time
// (e.g. the first one in inRange) so the tests are predictable.

Critter* NearestToTowerStrategy::choose(const Tower& tower, const std::vector<Critter*>& inRange) const {
    Critter* nearestCritter = nullptr;
    for (Critter* c : inRange) {
        if (c == nullptr || !c->isAlive()) { continue; }
        if (nearestCritter == nullptr || c->distanceTo(tower.getRow(), tower.getCol()) < nearestCritter->distanceTo(tower.getRow(), tower.getCol())) {
            nearestCritter = c;
        }
    }
    return nearestCritter;
}

std::string NearestToTowerStrategy::getName() const {
    return "NEAREST TO TOWER";
}

Critter* NearestToExitStrategy::choose(const Tower& tower, const std::vector<Critter*>& inRange) const {
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

std::string NearestToExitStrategy::getName() const {
    return "NEAREST TO EXIT";
}

Critter* StrongestStrategy::choose(const Tower& tower, const std::vector<Critter*>& inRange) const {
    Critter* strongestCritter = nullptr;
    for (Critter* c : inRange) {
        if (c == nullptr || !c->isAlive()) { continue; }
        if (strongestCritter == nullptr ||c->getHitPoints() > strongestCritter->getHitPoints()) {
            strongestCritter = c;
        }
    }
    return strongestCritter;
}

std::string StrongestStrategy::getName() const {
    return "STRONGEST";
}

Critter* WeakestStrategy::choose(const Tower& tower, const std::vector<Critter*>& inRange) const {
    Critter* weakestCritter = nullptr;
    for (Critter* c : inRange) {
        if (c == nullptr || !c->isAlive()) { continue; }
        if (weakestCritter == nullptr ||c->getHitPoints() < weakestCritter->getHitPoints()) {
            weakestCritter = c;
        }
    }
    return weakestCritter;
}

std::string WeakestStrategy::getName() const {
    return "WEAKEST";
}