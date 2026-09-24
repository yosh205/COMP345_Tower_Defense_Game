/**
 * @file MapView.h
 * @brief Declares MapView, which draws a Map in an SFML window.
 */
#ifndef MapView_h
#define MapView_h

#include <SFML/Graphics.hpp>
#include "../Map/Map.h"

/**
 * @brief Draws a Map as a grid of coloured squares.
 *
 * MapView only reads the map (through const Map&), so drawing can never
 * change the game state. Colours: green = scenery, sand = path,
 * blue = entry, red = exit. Cells on the critter route are marked with a dot.
 */
class MapView {
public:
    /** @param cellSize Width and height of one cell, in pixels. */
    explicit MapView(float cellSize = 48.f);

    /**
     * @brief Draws every cell of the map, then marks the route from findPath().
     * @param window The window to draw into.
     * @param map    The map to draw.
     */
    void draw(sf::RenderWindow& window, const Map& map) const;

    /** @return Size of one cell, in pixels. */
    float getCellSize() const;

private:
    float cellSize; ///< Width and height of one cell, in pixels.

    /** @brief Returns the fill colour for a cell type. */
    sf::Color colorFor(CellType type) const;
};

#endif // MapView_h
