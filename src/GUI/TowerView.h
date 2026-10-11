/**
 * @file TowerView.h
 * @brief Draws towers with distinct shapes and a range ring when selected.
 */
#ifndef TowerView_h
#define TowerView_h

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "../Tower/Tower.h"

/**
 * @brief Draws every tower with a shape that shows its type, facing its target.
 *
 * Direct = dark turret with a barrel, Area = orange mortar with four ticks,
 * Slow = blue crystal. Yellow dots under a tower show its level. The selected
 * tower is outlined in yellow and shows its range as a circle. TowerView only
 * reads the towers, so drawing never changes the game state.
 */
class TowerView {
public:
    /**
     * @brief Draws all towers.
     * @param window   The window to draw into.
     * @param towers   The towers on the map.
     * @param cellSize Size of one map cell, in pixels.
     * @param originX  Left edge of the map, in pixels.
     * @param originY  Top edge of the map, in pixels.
     * @param selected Index of the selected tower in towers, or -1 for none.
     */
    void draw(sf::RenderWindow& window,
              const std::vector<std::unique_ptr<Tower>>& towers,
              float cellSize,
              float originX,
              float originY,
              int selected) const;

private:
    /**
     * @brief Draws one tower, rotated to Tower::getFacingDegrees(), plus its range if selected.
     * @param window   The window to draw into.
     * @param tower    The tower to draw.
     * @param cellSize Size of one map cell, in pixels.
     * @param originX  Left edge of the map, in pixels.
     * @param originY  Top edge of the map, in pixels.
     * @param selected True to draw the selection outline and range circle.
     */
    void drawOne(sf::RenderWindow& window, const Tower& tower,
                 float cellSize, float originX, float originY,
                 bool selected) const;
};

#endif // TowerView_h
