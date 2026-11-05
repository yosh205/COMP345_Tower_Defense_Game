/**
 * @file MapLibrary.h
 * @brief Hand-built valid maps the player can choose on the start menu.
 */
#ifndef MapLibrary_h
#define MapLibrary_h

#include <string>
#include <vector>

#include "Map.h"

/** @brief A playable map with the name shown in the start menu. */
struct NamedMap {
    std::string name;  ///< Short title, e.g. "SNAKE". Only characters the bitmap font can draw.
    Map map;           ///< The map itself; always valid.
};

/**
 * @brief Builds a map from rows of text, one character per cell.
 *
 * 'E' is the Entry, 'X' the Exit, '.' a Path cell and '#' Scenery.
 * The map has one row per string and as many columns as the strings are long.
 * @param rows The rows, top to bottom. All must have the same length.
 * @return The map. It is not checked for validity: call Map::isValid().
 * @throws std::invalid_argument if there are no rows, the rows have different
 *         lengths, or a row contains another character.
 */
Map makeMapFromRows(const std::vector<std::string>& rows);

/**
 * @brief Builds the maps the player can choose, easiest first.
 *
 * Every map is valid, and every path cell is on the route from the Entry to
 * the Exit: path segments never touch, so critters cannot take a shortcut.
 * @return The maps, in the order the start menu shows them.
 */
std::vector<NamedMap> makePlayableMaps();

#endif // MapLibrary_h
