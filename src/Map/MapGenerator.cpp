/**
 * @file MapGenerator.cpp
 * @brief Random single-path map builder. Uses only Map's blank ctor and setCell.
 *
 * Builds one simple (non-branching) route from Entry to Exit so every path
 * cell is walkable by critters (same cells as findPath()). Matches Part 1:
 * blank map of given size, setCell for contents, connected entry→exit path.
 */
#include "MapGenerator.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <vector>

namespace {

/** @brief Writes only the cells on `route` (first = Entry, last = Exit). */
void applyRoute(Map& map, const std::vector<Position>& route) {
    for (int r = 0; r < map.getLength(); ++r) {
        for (int c = 0; c < map.getWidth(); ++c) {
            map.setCell(r, c, CellType::Scenery);
        }
    }
    if (route.empty()) {
        return;
    }
    for (const Position& p : route) {
        map.setCell(p.row, p.col, CellType::Path);
    }
    map.setCell(route.front().row, route.front().col, CellType::Entry);
    map.setCell(route.back().row, route.back().col, CellType::Exit);
}

/**
 * @brief Builds one simple path: left→right with random vertical jogs.
 *
 * Never branches or loops, so every path cell lies on the unique route
 * critters take from Entry to Exit.
 */
std::vector<Position> buildSimplePath(int length, int width,
                                      Position entry, Position exit,
                                      std::mt19937& rng) {
    std::vector<Position> route;
    route.reserve(static_cast<std::size_t>(length * width));

    int r = entry.row;
    int c = entry.col;
    route.push_back({r, c});

    std::uniform_int_distribution<int> rowDist(0, length - 1);
    std::uniform_int_distribution<int> coin(0, 99);

    // Advance column by column; optionally jog vertically before stepping right.
    while (c < width - 1) {
        // Pick a target row for this column (biased toward the eventual exit).
        int targetRow = rowDist(rng);
        if (coin(rng) < 40) {
            targetRow = exit.row;
        }

        // Limit how far we jog so paths stay readable.
        const int maxJog = std::max(1, length / 2);
        int jogBudget = maxJog;
        while (r != targetRow && jogBudget-- > 0) {
            r += (targetRow > r) ? 1 : -1;
            route.push_back({r, c});
        }

        ++c;
        route.push_back({r, c});
    }

    // Final column: move vertically to the exit row.
    while (r != exit.row) {
        r += (exit.row > r) ? 1 : -1;
        route.push_back({r, c});
    }

    return route;
}

} // namespace

Map generateRandomMap(int length, int width) {
    // Part 1: blank map given length and width; fill only via setCell.
    Map map(length, width);

    std::mt19937 rng(static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> rowDist(0, length - 1);

    const Position entry{rowDist(rng), 0};
    const Position exit{rowDist(rng), width - 1};

    const std::vector<Position> route =
        buildSimplePath(length, width, entry, exit, rng);
    applyRoute(map, route);
    return map;
}
