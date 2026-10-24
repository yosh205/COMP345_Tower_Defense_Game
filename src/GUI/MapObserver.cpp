/**
 * @file MapObserver.cpp
 * @brief Implements MapObserver. Function docs are in MapObserver.h.
 */
#include "MapObserver.h"

#include <iostream>
#include <algorithm>

MapObserver::MapObserver(Map& map, sf::RenderWindow& window) : map(&map), window(&window), view(40.f) {
    this->map->attach(this); // plug in
    render(); // show the map
}

MapObserver::~MapObserver() {
    map->detach(this); // unplug
}

void MapObserver::update(Observable* subject) {
    if (subject != map) {
        return;  // not my map: ignore
    }
    ++updateCount;
    std::cout << "MapObserver: update() called " << updateCount << " times\n";
    render();
}


void MapObserver::render() {
    // Fit the map in the windows with square cells, centered.
    const float winWidth = static_cast<float>(window->getSize().x);
    const float winHeight = static_cast<float>(window->getSize().y);
    const float rows = static_cast<float>(map->getLength());
    const float cols = static_cast<float>(map->getWidth());
    const float cellSize = std::min(winWidth / cols, winHeight / rows);
    const float originX = (winWidth - cols * cellSize) * 0.5f;
    const float originY = (winHeight - rows * cellSize) * 0.5f;
    view.setCellSize(cellSize);
    view.setOrigin(originX, originY);

    window->clear(sf::Color(12,16,22));
    view.draw(*window, *map);

    //Towers recorded in the map
    sf::CircleShape tower(cellSize * 0.3f);
    tower.setOrigin(tower.getRadius(), tower.getRadius());
    tower.setFillColor(sf::Color(45, 48, 55));
    tower.setOutlineThickness(2.f);
    tower.setOutlineColor(sf::Color(255,220,80));
    for (int row = 0; row < map->getLength(); ++row) {
        for (int col = 0; col < map->getWidth(); ++col) {
            if (map->hasTower(row, col)) {
                tower.setPosition(originX + (col+0.5f) * cellSize, originY + (row+0.5f) * cellSize);
                window->draw(tower);
            }
        }
    }

    window->display();
}

int MapObserver::getUpdateCount() const {
    return updateCount;
}
