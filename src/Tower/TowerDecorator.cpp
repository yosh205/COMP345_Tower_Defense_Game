/**
 * @file TowerDecorator.cpp
 * @brief Implements the tower decorators. Function docs are in TowerDecorator.h.
 */
#include "TowerDecorator.h"

// TODO(A3 Part 3): implement the constructors and the overrides of
//   TowerDecorator     (forwards everything to inner)
//   LevelUpDecorator   (level + 1; new costs, refund, range, power, fire rate)
//   SplashDecorator    (after a hit, damage critters within a radius of the target)
//   BurningDecorator   (after a hit, the target burns for a few seconds)
//   FreezingDecorator  (after a hit, Critter::applySlow() on the target)
// Then use them in main.cpp: Upgrade wraps the selected tower in a LevelUpDecorator,
// and new sidebar buttons add Splash / Burning / Freezing for a price.
