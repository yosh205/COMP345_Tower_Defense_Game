/**
 * @file MapLibrary.cpp
 * @brief Implements makeMapFromRows() and makePlayableMaps(). Function docs are in MapLibrary.h.
 */
#include "MapLibrary.h"

#include <stdexcept>

Map makeMapFromRows(const std::vector<std::string>& rows) {
    if (rows.empty() || rows.front().empty()) {
        throw std::invalid_argument("makeMapFromRows: the map needs at least one cell");
    }
    const int length = static_cast<int>(rows.size());
    const int width = static_cast<int>(rows.front().size());

    Map map(length, width);
    for (int row = 0; row < length; ++row) {
        const std::string& line = rows[static_cast<std::size_t>(row)];
        if (static_cast<int>(line.size()) != width) {
            throw std::invalid_argument("makeMapFromRows: all rows must have the same length");
        }
        for (int col = 0; col < width; ++col) {
            switch (line[static_cast<std::size_t>(col)]) {
                case '#': break;  // a new map is already all scenery
                case '.': map.setCell(row, col, CellType::Path); break;
                case 'E': map.setCell(row, col, CellType::Entry); break;
                case 'X': map.setCell(row, col, CellType::Exit); break;
                default:
                    throw std::invalid_argument("makeMapFromRows: use only E, X, '.' and '#'");
            }
        }
    }
    return map;
}

std::vector<NamedMap> makePlayableMaps() {
    std::vector<NamedMap> maps;

    // One bend: a long straight path, easy to defend.
    maps.push_back({"EASY BEND", makeMapFromRows({
        "################",
        "################",
        "E.......########",
        "#######.########",
        "#######.########",
        "#######........X",
        "################",
        "################",
    })});

    // Back and forth across the map: towers in the middle reach several rows.
    maps.push_back({"SNAKE", makeMapFromRows({
        "##############",
        "E...........##",
        "###########.##",
        "##..........##",
        "##.###########",
        "##..........##",
        "###########.##",
        "##..........##",
        "##.###########",
        "##X###########",
    })});

    // Up and down in columns.
    maps.push_back({"ZIGZAG", makeMapFromRows({
        "#E############",
        "#.##....##...#",
        "#.##.##.##.#.#",
        "#.##.##.##.#.#",
        "#.##.##.##.#.#",
        "#.##.##.##.#.#",
        "#.##.##.##.#.#",
        "#.##.##.##.#.#",
        "#....##....#.#",
        "############X#",
    })});

    // Around the edge, then inwards to the exit in the middle.
    maps.push_back({"SPIRAL", makeMapFromRows({
        "##############",
        "E............#",
        "############.#",
        "#..........#.#",
        "#.########.#.#",
        "#.########.#.#",
        "#.#X.......#.#",
        "#.##########.#",
        "#............#",
        "##############",
    })});

    return maps;
}
