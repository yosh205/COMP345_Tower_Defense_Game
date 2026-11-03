/**
 * @file MapEditor.cpp
 * @brief Implements MapEditor. Function docs are in MapEditor.h.
 */
#include "MapEditor.h"

MapEditor::MapEditor(int length, int width) : map(length, width) {}

MapEditor::MapEditor(const Map& start) : map(start) {}

void MapEditor::setTool(EditorTool newTool) { tool = newTool; }

EditorTool MapEditor::getTool() const { return tool; }

bool MapEditor::paint(int row, int col) {
    if (!map.isInBounds(row, col)) {
        return false;
    }
    CellType type = CellType::Path;
    switch (tool) {
        case EditorTool::Path:    type = CellType::Path; break;
        case EditorTool::Entry:   type = CellType::Entry; break;
        case EditorTool::Exit:    type = CellType::Exit; break;
        case EditorTool::Scenery: type = CellType::Scenery; break;
    }
    if (map.getCellType(row, col) == type) {
        return false;
    }
    // Only one Entry and one Exit: placing a new one moves it.
    if (type == CellType::Entry || type == CellType::Exit) {
        removeOthers(type, row, col);
    }
    map.setCell(row, col, type);
    return true;
}

bool MapEditor::erase(int row, int col) {
    if (!map.isInBounds(row, col) || map.getCellType(row, col) == CellType::Scenery) {
        return false;
    }
    map.setCell(row, col, CellType::Scenery);
    return true;
}

void MapEditor::clear() {
    map = Map(map.getLength(), map.getWidth());
}

const Map& MapEditor::getMap() const { return map; }

bool MapEditor::isValid() const { return map.isValid(); }

std::string MapEditor::getProblem() const {
    if (count(CellType::Entry) == 0) {
        return "PLACE AN ENTRY";
    }
    if (count(CellType::Exit) == 0) {
        return "PLACE AN EXIT";
    }
    if (!map.isValid()) {
        return "CONNECT THE ENTRY TO\nTHE EXIT WITH PATH CELLS";
    }
    return "";
}

int MapEditor::getUnusedPathCells() const {
    if (!map.isValid()) {
        return 0;
    }
    const int walkable = count(CellType::Path) + count(CellType::Entry) + count(CellType::Exit);
    return walkable - static_cast<int>(map.findPath().size());
}

void MapEditor::removeOthers(CellType type, int row, int col) {
    for (int r = 0; r < map.getLength(); ++r) {
        for (int c = 0; c < map.getWidth(); ++c) {
            if ((r != row || c != col) && map.getCellType(r, c) == type) {
                map.setCell(r, c, CellType::Scenery);
            }
        }
    }
}

int MapEditor::count(CellType type) const {
    int total = 0;
    for (int r = 0; r < map.getLength(); ++r) {
        for (int c = 0; c < map.getWidth(); ++c) {
            total += map.getCellType(r, c) == type ? 1 : 0;
        }
    }
    return total;
}
