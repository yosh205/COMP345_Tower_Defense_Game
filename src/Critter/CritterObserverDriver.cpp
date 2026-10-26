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

    const int kCount = 3;
    Critter* roster[kCount] = {&normal, &armored, &fast};
    int pathIndex[kCount] = {0, 3, 6};
    bool onMap[kCount] = {false, false, false};  // true while the observer watches roster[i]

    // Starting copies, so a critter added again comes back alive at its first cell
    // instead of still dead (dead critters are not drawn).
    const Critter fresh[kCount] = {normal, armored, fast};
    const int startIndex[kCount] = {0, 3, 6};

    CritterObserver observer(std::cout);
    observer.showOn(window, map);

    std::cout << "\nCritter observer demo. Adding, removing or changing a critter redraws the view.\n"
              << "Keys: A add a critter, R remove one (dead ones first), D damage a living one, "
                 "M move them, Esc back\n";

    // One critter, plugged in. plugInto() attaches the observer and redisplays the view.
    observer.plugInto(normal);
    onMap[0] = true;

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
                    case sf::Keyboard::A: {
                        // The first critter not on the map.
                        int i = 0;
                        while (i < kCount && onMap[i]) {
                            ++i;
                        }
                        // Critters move in step, so one added on another's cell would stay
                        // hidden under it.
                        bool startTaken = false;
                        for (int j = 0; i < kCount && j < kCount; ++j) {
                            if (onMap[j] && roster[j]->isAlive() && pathIndex[j] == startIndex[i]) {
                                startTaken = true;
                            }
                        }
                        if (startTaken) {
                            std::cout << "A: another critter is on its start cell, press M first\n";
                        } else if (i < kCount) {
                            // Not watched yet, so this reset notifies nobody; plugInto() shows it.
                            *roster[i] = fresh[i];
                            pathIndex[i] = startIndex[i];
                            observer.plugInto(*roster[i]);
                            onMap[i] = true;
                            std::cout << "A: critter added (" << observer.getWatchedCount()
                                      << " watching)\n";
                        } else {
                            std::cout << "A: all three critters are already on the map\n";
                        }
                        break;
                    }
                    case sf::Keyboard::R: {
                        // A dead critter if there is one (it is no longer drawn), otherwise the last one.
                        int i = -1;
                        for (int j = 0; j < kCount; ++j) {
                            if (onMap[j] && !roster[j]->isAlive()) {
                                i = j;
                                break;
                            }
                        }
                        for (int j = kCount - 1; i == -1 && j >= 0; --j) {
                            if (onMap[j]) {
                                i = j;
                            }
                        }
                        if (i != -1) {
                            observer.unplug(*roster[i]);
                            onMap[i] = false;
                            std::cout << "R: critter removed (" << observer.getWatchedCount()
                                      << " watching)\n";
                        } else {
                            std::cout << "R: no critter to remove\n";
                        }
                        break;
                    }
                    case sf::Keyboard::D: {
                        // The first living critter on the map.
                        int i = 0;
                        while (i < kCount && !(onMap[i] && roster[i]->isAlive())) {
                            ++i;
                        }
                        if (i < kCount) {
                            // takeDamage() notifies the observer, which redisplays the view.
                            roster[i]->takeDamage(5);
                            std::cout << "D: damaged a critter"
                                      << (roster[i]->isAlive() ? "\n" : ", it died (R removes it)\n");
                        } else {
                            std::cout << "D: no living critter to damage\n";
                        }
                        break;
                    }
                    case sf::Keyboard::M:
                        if (observer.getWatchedCount() == 0) {
                            std::cout << "M: no critter to move\n";
                            break;
                        }
                        for (int i = 0; i < kCount; ++i) {
                            if (!onMap[i] || !roster[i]->isAlive()) {
                                continue;
                            }
                            if (pathIndex[i] + 1 < static_cast<int>(path.size())) {
                                ++pathIndex[i];
                                const Position& cell = path[static_cast<std::size_t>(pathIndex[i])];
                                // setPosition() notifies; the observer redraws this critter.
                                roster[i]->setPosition(static_cast<float>(cell.row),
                                                       static_cast<float>(cell.col));
                            } else {
                                // Already on the exit: it leaves the map, as in the game.
                                observer.unplug(*roster[i]);
                                onMap[i] = false;
                                std::cout << "M: a critter reached the exit and was removed\n";
                            }
                        }
                        std::cout << "M: moved the watched critters along the path ("
                                  << observer.getWatchedCount() << " watching)\n";
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
