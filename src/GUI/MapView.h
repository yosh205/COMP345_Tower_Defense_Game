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
    explicit MapView(float cellSize = 48.f);

    void draw(sf::RenderWindow& window, const Map& map) const;

    float getCellSize() const;
    void setCellSize(float size);

    /** @brief Top-left of the map in window pixels (for centering). */
    void setOrigin(float x, float y);
    float getOriginX() const;
    float getOriginY() const;

private:
    float cellSize;
    float originX = 0.f;
    float originY = 0.f;

    sf::Color colorFor(CellType type) const;
};

#endif // MapView_h
