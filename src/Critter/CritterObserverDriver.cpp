/**
 * @file CritterObserverDriver.cpp
 * @brief Driver: critters + CritterObserver. Add, remove, damage and move each redraw the view.
 */
#include "CritterObserverDriver.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "../Map/MapExamples.h"
#include "CritterObserver.h"

void CritterObserverDriver::run() const {
    sf::RenderWindow window(sf::VideoMode(700, 500),
                            "Critter Observer Demo - A add, R remove, D damage, M move, Esc back");
    window.setFramerateLimit(30);

    // The map is only the background. Critters are the observable subject.
    // Declared before the observer so they are still alive when it detaches.
    Map map = makeMapExamples().front().map;
    const std::vector<Position> path = map.findPath();
    if (path.size() < 7) {
        std::cout << "Critter observer demo: the map has no path to place critters on.\n";
        return;
    }

    Critter normal(CritterKind::Normal, 30, 1.0f, path[0].row, path[0].col, 1);
    Critter armored(CritterKind::Armored, 45, 0.8f, path[3].row, path[3].col, 2);
    Critter fast(CritterKind::Fast, 20, 1.6f, path[6].row, path[6].col, 1);

    Critter* roster[] = {&normal, &armored, &fast};
    int pathIndex[] = {0, 3, 6};
    int added = 0;

    CritterObserver observer(std::cout);
    observer.showOn(window, map);

    std::cout << "\nCritter observer demo. Adding, removing or changing a critter redraws the view.\n"
              << "Keys: A add a critter, R remove the last one, D damage the first, M move them, Esc back\n";

    // One critter, plugged in. plugInto() attaches the observer and redisplays the view.
    observer.plugInto(normal);
    added = 1;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::Resized) {
                window.setView(sf::View(sf::FloatRect(0.f, 0.f,
                                                      static_cast<float>(event.size.width),
                                                      static_cast<float>(event.size.height))));
                observer.render();
            } else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::A:
                        if (added < 3) {
                            observer.plugInto(*roster[added]);
                            ++added;
                            std::cout << "A: critter added (" << added << " watching)\n";
                        } else {
                            std::cout << "A: all three critters are already on the map\n";
                        }
                        break;
                    case sf::Keyboard::R:
                        if (added > 0) {
                            --added;
                            observer.unplug(*roster[added]);
                            std::cout << "R: critter removed (" << added << " watching)\n";
                        } else {
                            std::cout << "R: no critter to remove\n";
                        }
                        break;
                    case sf::Keyboard::D:
                        if (added > 0) {
                            // takeDamage() notifies the observer, which redisplays the view.
                            roster[0]->takeDamage(5);
                            std::cout << "D: damaged the first critter\n";
                        } else {
                            std::cout << "D: no critter to damage\n";
                        }
                        break;
                    case sf::Keyboard::M:
                        if (added == 0) {
                            std::cout << "M: no critter to move\n";
                            break;
                        }
                        for (int i = 0; i < added; ++i) {
                            if (pathIndex[i] + 1 < static_cast<int>(path.size())) {
                                ++pathIndex[i];
                                const Position& cell = path[static_cast<std::size_t>(pathIndex[i])];
                                // setPosition() notifies; the observer redraws this critter.
                                roster[i]->setPosition(static_cast<float>(cell.row),
                                                       static_cast<float>(cell.col));
                            }
                        }
                        std::cout << "M: moved the watched critters along the path\n";
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

    std::cout << "Critter observer demo closed while watching " << observer.getWatchedCount()
              << " critter(s).\n\n";
}
