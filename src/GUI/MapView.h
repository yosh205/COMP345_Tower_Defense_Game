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
    /**
     * @brief Creates a view that draws cells of the given size.
     * @param cellSize Width and height of one cell, in pixels.
     */
    explicit MapView(float cellSize = 48.f);

    /**
     * @brief Draws every cell of the map, then a white dot on each cell of the
     * route from Map::findPath().
     * @param window The window to draw into.
     * @param map    The map to draw.
     */
    void draw(sf::RenderWindow& window, const Map& map) const;

    /** @return Width and height of one cell, in pixels. */
    float getCellSize() const;
    /**
     * @brief Changes the cell size, e.g. to fit the map in a resized window.
     * @param size Width and height of one cell, in pixels.
     */
    void setCellSize(float size);

    /**
     * @brief Top-left of the map in window pixels (for centering).
     * @param x Left edge of the map, in pixels.
     * @param y Top edge of the map, in pixels.
     */
    void setOrigin(float x, float y);
    /** @return Left edge of the map, in pixels. */
    float getOriginX() const;
    /** @return Top edge of the map, in pixels. */
    float getOriginY() const;

private:
    float cellSize;       ///< Width and height of one cell, in pixels.
    float originX = 0.f;  ///< Left edge of the map, in pixels.
    float originY = 0.f;  ///< Top edge of the map, in pixels.

    /**
     * @param type A cell type.
     * @return Fill colour for that cell type (green, sand, blue or red).
     */
    sf::Color colorFor(CellType type) const;
};

#endif // MapView_h
