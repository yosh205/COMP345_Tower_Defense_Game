/**
 * @file MapExamples.h
 * @brief Hand-built maps that show which maps Map::isValid() accepts and rejects.
 *
 * Used by the "Map rules demo" screen of the driver and by the tests. Each map
 * is built from a blank map with Map::setCell(), and all but the first break
 * exactly one of the map rules.
 */
#ifndef MapExamples_h
#define MapExamples_h

#include <string>
#include <vector>

#include "Map.h"

/** @brief One example map with a short name and the rule it shows. */
struct MapExample {
    std::string name;  ///< Short title, e.g. "TWO ENTRIES".
    std::string rule;  ///< The map rule this example shows (lines separated by '\n').
    Map map;           ///< The map itself.
};

/**
 * @brief Builds the example maps, valid one first.
 *
 * In order: a valid map, then maps with no exit, two entries, two exits,
 * a gap in the path, cells connected only diagonally, and a blank map.
 * The text only uses characters the game's bitmap font can draw (A-Z, 0-9, ':' …).
 * @return The examples; only the first one is valid.
 */
std::vector<MapExample> makeMapExamples();

#endif // MapExamples_h
