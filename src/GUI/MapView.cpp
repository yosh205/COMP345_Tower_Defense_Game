/**
 * @file MapView.cpp
 * @brief Implements MapView. Screen x comes from the column, y from the row.
 */
#include "MapView.h"

MapView::MapView(float cellSize) : cellSize(cellSize) {}

float MapView::getCellSize() const { return cellSize; }

sf::Color MapView::colorFor(CellType type) const {
    switch (type) {
        case CellType::Scenery: return sf::Color(70, 140, 70);    // green
        case CellType::Path:    return sf::Color(200, 170, 120);  // sand
        case CellType::Entry:   return sf::Color(60, 110, 220);   // blue
        case CellType::Exit:    return sf::Color(210, 60, 60);    // red
    }
    return sf::Color::Black;
}

void MapView::draw(sf::RenderWindow& window, const Map& map) const {
    // 1. One square per cell; the 1px gap leaves visible grid lines.
    sf::RectangleShape cell(sf::Vector2f(cellSize - 1.f, cellSize - 1.f));
    for (int row = 0; row < map.getLength(); ++row) {
        for (int col = 0; col < map.getWidth(); ++col) {
            cell.setPosition(col * cellSize, row * cellSize);  // x = col, y = row
            cell.setFillColor(colorFor(map.getCellType(row, col)));
            window.draw(cell);
        }
    }

    // 2. A dot in the centre of each cell on the route critters will follow.
    float radius = cellSize / 8.f;
    sf::CircleShape dot(radius);
    dot.setFillColor(sf::Color::White);
    for (const Position& p : map.findPath()) {
        dot.setPosition(p.col * cellSize + cellSize / 2.f - radius,
                        p.row * cellSize + cellSize / 2.f - radius);
        window.draw(dot);
    }
}
