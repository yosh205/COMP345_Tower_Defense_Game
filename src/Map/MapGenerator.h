/**
 * @file MapGenerator.h
 * @brief Builds a valid Map with a single random path using Map::setCell.
 */
#ifndef MapGenerator_h
#define MapGenerator_h

#include "Map.h"

/**
 * @brief Creates a blank map, then carves one simple path from Entry to Exit.
 *
 * Entry is on the left edge, Exit on the right. Only cells on that route are
 * Path/Entry/Exit — no dead-end or unused path cells — so every walkable cell
 * is on the critter route returned by findPath().
 *
 * @param length Number of rows (must be > 0).
 * @param width  Number of columns (must be > 0).
 * @return A valid Map matching Part 1 map rules.
 */
Map generateRandomMap(int length, int width);

#endif // MapGenerator_h
