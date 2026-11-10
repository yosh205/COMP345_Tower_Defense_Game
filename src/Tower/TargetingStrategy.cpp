/**
 * @file TargetingStrategy.cpp
 * @brief Implements the targeting strategies. Function docs are in TargetingStrategy.h.
 */
#include "TargetingStrategy.h"

// TODO(A3 Part 1): implement choose() and getName() for
//   NearestToTowerStrategy  (smallest Critter::distanceTo(tower row, tower col))
//   NearestToExitStrategy   (largest Critter::getProgress(); same as Tower::selectTarget() today)
//   StrongestStrategy       (most Critter::getHitPoints())
//   WeakestStrategy         (fewest Critter::getHitPoints())
// Each returns nullptr when inRange is empty. Break ties the same way every time
// (e.g. the first one in inRange) so the tests are predictable.
