/**
 * @file MapObserverDriver.cpp
 * @brief Implements MapObserverDriver. Function docs are in MapObserverDriver.h.
 */
#include "MapObserverDriver.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

#include "../Map/MapExamples.h"
#include "MapObserver.h"

namespace {

    /** @brief Prints what a key did and whether the map actually changed. */
    void report(const std::string& action, bool changed) {
        std::cout << " " << action << (changed ? " map changed\n" : " not allowed, map unchanged (no update)\n");
    }
} // namespace

void MapObserverDriver::run() const {
    sf::RenderWindow window(sf::VideoMode(700, 500), "Map Observer Demo - P place, M move, R remove, C cell, N reset, Esc back");
    window.setFramerateLimit(30);

    // The subject: the valid 5 x 7 example map.
    // Declared before the observer, so the map outlives it
    Map map = makeMapExamples().front().map;

    // The observer: attaches itself to the map and draws it once.
    MapObserver observer(map, window);

    std::cout << "\nMap observer demo. Each change redraws the map through update().\n"
              << "Keys: P place tower, M move tower, R remove tower, C change a cell, N reset map, Esc back\n";

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::Resized) {
                // A window change, not a map change: redraw directly, no notify().
                window.setView(sf::View(sf::FloatRect(0.f, 0.f,
                                        static_cast<float>(event.size.width),
                                        static_cast<float>(event.size.height))));
                observer.render();
            } else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::P:
                        report("P: place tower at (0,3)", map.placeTower(0, 3));
                        break;
                    case sf::Keyboard::M:
                        if (map.hasTower(0, 3)) {
                            report("M: move tower (0, 3) -> (2, 4)", map.moveTower(0, 3, 2, 4));
                        } else {
                            report("M: move tower (2, 4) -> (0, 3)", map.moveTower(2, 4, 0, 3));
                        }
                        break;
                    case sf::Keyboard::R:
                        report("R: remove tower", map.removeTower(0, 3) || map.removeTower(2,4));
                        break;
                    case sf::Keyboard::C: {
                        const CellType next = (map.getCellType(4, 3) == CellType::Scenery) ? CellType::Path : CellType::Scenery;
                        report("C: toggle cell (4,3)", map.setCell(4, 3, next));
                        break;
                    }
                    case sf::Keyboard::N:
                        map = makeMapExamples().front().map;  // Map::operator= notifies
                        report("N: reset the map", true);
                        break;
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                    default:
                        break;
                }
            }
        }
    }

    std::cout << "Map observer demo closed after " << observer.getUpdateCount() << " updates.\n\n";
}
