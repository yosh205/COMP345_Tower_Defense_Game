/**
 * @file Map.cpp
 * @brief Implements the Map class declared in Map.h.
 *
 * Implementation notes:
 * - The grid is stored as a vector of rows and accessed as grid[row][col].
 * - Every query that reads a cell checks isInBounds() first, so positions
 *   outside the map are handled safely instead of reading out of range.
 * - findPath() uses a breadth-first search (BFS) over walkable cells, moving
 *   up, down, left or right. BFS explores cells in order of distance from
 *   the entry, so the first route found to the exit is a shortest one.
 * - isValid() reuses findPath() for the connectivity rule and counts the
 *   Entry and Exit cells separately for the "exactly one" rules.
 *
 * Function documentation is in Map.h.
 */
#include "Map.h"

#include <algorithm>
#include <queue>
#include <stdexcept>

// --- Construction ---

Map::Map(int length, int width) : length(length), width(width) {
    // Reject impossible sizes so every Map object has at least one cell.
    if (length <= 0 || width <= 0) {
        throw std::invalid_argument("Map dimensions must be greater than 0.");
    }
    // length rows, each holding width Scenery cells (a blank map)
    grid = std::vector<std::vector<CellType>>(length, std::vector<CellType>(width, CellType::Scenery));
}

// --- Getters ---

int Map::getLength() const { return length; }

int Map::getWidth() const { return width; }

CellType Map::getCellType(int row, int col) const { return grid[row][col]; }

// --- Setters ---

bool Map::setCell(int row, int col, CellType type) {
    if (!isInBounds(row, col)) {
        return false;
    }
    grid[row][col] = type;
    return true;
}

// --- Queries ---

bool Map::isInBounds(int row, int col) const {
    return row >= 0 && row < length && col >= 0 && col < width;
}

bool Map::canPlaceTower(int row, int col) const {
    return isInBounds(row, col) && grid[row][col] == CellType::Scenery;
}

bool Map::isWalkable(int row, int col) const {
    return isInBounds(row, col) && grid[row][col] != CellType::Scenery;
}

// --- Validation ---

bool Map::isValid() const {
    // Count entries and exits: findPath() alone would not notice duplicates.
    int entryCount = 0;
    int exitCount = 0;
    for (int r = 0; r < length; r++) {
        for (int c = 0; c < width; c++) {
            if (grid[r][c] == CellType::Entry) entryCount++;
            if (grid[r][c] == CellType::Exit) ++exitCount;
        }
    }
    return entryCount == 1 && exitCount == 1 && !findPath().empty();
}

std::vector<Position> Map::findPath() const {
    // 1. Locate the entry and exit. {-1, -1} means "not found".
    Position entry{-1, -1};
    Position exit{-1, -1};
    for (int r = 0; r < length; ++r) {
        for (int c = 0; c < width; ++c) {
            if (grid[r][c] == CellType::Entry) entry = {r, c};
            if (grid[r][c] == CellType::Exit) exit = {r, c};
        }
    }
    if (entry.row == -1 || exit.row == -1) {
        return {};
    }

    // 2. Breadth-first search from the entry over walkable cells.
    //    visited:  cells already found, so none is explored twice.
    //    cameFrom: the cell each cell was reached from, used in step 4.
    //    toVisit:  cells found but not yet explored, oldest first.
    std::vector<std::vector<bool>> visited(length, std::vector<bool>(width, false));
    std::vector<std::vector<Position>> cameFrom(length, std::vector<Position>(width, Position{-1, -1}));
    std::queue<Position> toVisit;

    toVisit.push(entry);
    visited[entry.row][entry.col] = true;

    const int dRow[] = {-1, 1, 0, 0}; // up, down, left, right
    const int dCol[] = {0, 0, -1, 1};

    while(!toVisit.empty()) {
        Position current = toVisit.front();
        toVisit.pop();

        if (current.row == exit.row && current.col == exit.col) {
            break;
        }

        for (int i = 0; i < 4; ++i) {
            int nextRow = current.row + dRow[i];
            int nextCol = current.col + dCol[i];
            if (isWalkable(nextRow, nextCol) && !visited[nextRow][nextCol]) {
                visited[nextRow][nextCol] = true;
                cameFrom[nextRow][nextCol] = current;
                toVisit.push({nextRow, nextCol});
            }
        }
    }

    // 3. The search ended without reaching the exit: the path has a gap.
    if (!visited[exit.row][exit.col]) {
        return {};
    }

    // 4. Follow cameFrom back from the exit. The entry's cameFrom is {-1, -1},
    //    which ends the loop. Reverse the result to get Entry -> Exit.
    std::vector<Position> path;
    for (Position p = exit; p.row != -1; p = cameFrom[p.row][p.col]) {
        path.push_back(p);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

