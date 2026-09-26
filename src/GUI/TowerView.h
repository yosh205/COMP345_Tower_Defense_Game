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

class TowerView {
public:
    void draw(sf::RenderWindow& window,
              const std::vector<std::unique_ptr<Tower>>& towers,
              float cellSize,
              float originX,
              float originY,
              int selected) const;

private:
    void drawOne(sf::RenderWindow& window, const Tower& tower,
                 float cellSize, float originX, float originY,
                 bool selected) const;
};

#endif // TowerView_h
