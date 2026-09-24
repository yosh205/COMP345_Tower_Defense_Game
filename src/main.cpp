/**
 * @file main.cpp
 * @brief Driver: shows valid and invalid maps in an SFML window.
 *
 * Controls: press 1-4 to switch maps, close the window to quit.
 */
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "Map/Map.h"
#include "GUI/MapView.h"

/** @brief A named map used in the demo. */
struct DemoMap {
    std::string name; ///< Description shown in the title bar.
    Map map;          ///< The map itself.
};

/** @brief Builds a valid 8x12 map with a winding path. */
Map makeValidMap() {
    Map map(8, 12);
    map.setCell(1, 0, CellType::Entry);
    for (int c = 1; c <= 5; ++c) map.setCell(1, c, CellType::Path);   // right
    for (int r = 2; r <= 6; ++r) map.setCell(r, 5, CellType::Path);   // down
    for (int c = 6; c <= 10; ++c) map.setCell(6, c, CellType::Path);  // right
    map.setCell(6, 11, CellType::Exit);
    return map;
}

int main() {
    // Each invalid map is the valid one with a single rule broken.
    Map noExit = makeValidMap();
    noExit.setCell(6, 11, CellType::Path);

    Map twoEntries = makeValidMap();
    twoEntries.setCell(4, 0, CellType::Entry);

    Map gap = makeValidMap();
    gap.setCell(4, 5, CellType::Scenery);

    std::vector<DemoMap> maps = {
        {"1: Valid map", makeValidMap()},
        {"2: No exit", noExit},
        {"3: Two entries", twoEntries},
        {"4: Gap in path", gap},
    };

    MapView view(48.f);
    const Map& first = maps[0].map;
    sf::RenderWindow window(
        sf::VideoMode(static_cast<unsigned>(first.getWidth() * view.getCellSize()),
                      static_cast<unsigned>(first.getLength() * view.getCellSize())),
        "Map demo");

    std::size_t current = 0;
    auto showCurrent = [&]() {
        bool valid = maps[current].map.isValid();
        std::string status = valid ? "VALID" : "INVALID";
        window.setTitle(maps[current].name + "  -  " + status + "  (press 1-4)");
        std::cout << maps[current].name << ": " << status << "\n";
    };
    showCurrent();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed &&
                       event.key.code >= sf::Keyboard::Num1 &&
                       event.key.code <= sf::Keyboard::Num4) {
                current = event.key.code - sf::Keyboard::Num1;
                showCurrent();
            }
        }
        window.clear();
        view.draw(window, maps[current].map);
        window.display();
    }
    return 0;
}
