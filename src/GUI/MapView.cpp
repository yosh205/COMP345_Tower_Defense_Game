/**
 * @file MapView.cpp
 * @brief Implements MapView. Screen x comes from the column, y from the row.
 */
#include "MapView.h"

#include <algorithm>

MapView::MapView(float cellSize) : cellSize(cellSize) {}

float MapView::getCellSize() const { return cellSize; }
void MapView::setCellSize(float size) { cellSize = size; }

void MapView::setOrigin(float x, float y) {
    originX = x;
    originY = y;
}
float MapView::getOriginX() const { return originX; }
float MapView::getOriginY() const { return originY; }

sf::Color MapView::colorFor(CellType type) const {
    switch (type) {
        case CellType::Scenery: return sf::Color(70, 140, 70);
        case CellType::Path:    return sf::Color(200, 170, 120);
        case CellType::Entry:   return sf::Color(60, 110, 220);
        case CellType::Exit:    return sf::Color(210, 60, 60);
    }
    return sf::Color::Black;
}

void MapView::draw(sf::RenderWindow& window, const Map& map) const {
    const float gap = std::max(1.f, cellSize * 0.04f);
    sf::RectangleShape cell(sf::Vector2f(cellSize - gap, cellSize - gap));
    for (int row = 0; row < map.getLength(); ++row) {
        for (int col = 0; col < map.getWidth(); ++col) {
            cell.setPosition(originX + col * cellSize, originY + row * cellSize);
            cell.setFillColor(colorFor(map.getCellType(row, col)));
            window.draw(cell);
        }
    }

    float radius = cellSize / 8.f;
    sf::CircleShape dot(radius);
    dot.setFillColor(sf::Color::White);
    for (const Position& p : map.findPath()) {
        dot.setPosition(originX + p.col * cellSize + cellSize / 2.f - radius,
                        originY + p.row * cellSize + cellSize / 2.f - radius);
        window.draw(dot);
    }
}
