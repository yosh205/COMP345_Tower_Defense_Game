/**
 * @file MapExamples.cpp
 * @brief Implements makeMapExamples(). Function docs are in MapExamples.h.
 */
#include "MapExamples.h"

namespace {

/**
 * @brief The valid 5 x 7 example: a path that bends from the entry to the exit.
 *
 *     # # # # # # #
 *     E . . # # # #
 *     # # . # # # #
 *     # # . . . . X
 *     # # # # # # #
 */
Map makeValidExample() {
    Map map(5, 7);
    map.setCell(1, 0, CellType::Entry);
    map.setCell(1, 1, CellType::Path);
    map.setCell(1, 2, CellType::Path);
    map.setCell(2, 2, CellType::Path);
    map.setCell(3, 2, CellType::Path);
    map.setCell(3, 3, CellType::Path);
    map.setCell(3, 4, CellType::Path);
    map.setCell(3, 5, CellType::Path);
    map.setCell(3, 6, CellType::Exit);
    return map;
}

} // namespace

std::vector<MapExample> makeMapExamples() {
    std::vector<MapExample> examples;

    examples.push_back({"VALID MAP",
                        "ONE ENTRY AND ONE EXIT\nCONNECTED BY PATH CELLS",
                        makeValidExample()});

    // Each invalid map is the valid one with a single rule broken.
    Map noExit = makeValidExample();
    noExit.setCell(3, 6, CellType::Path);
    examples.push_back({"NO EXIT", "A MAP NEEDS EXACTLY\nONE EXIT", noExit});

    Map twoEntries = makeValidExample();
    twoEntries.setCell(4, 0, CellType::Entry);
    examples.push_back({"TWO ENTRIES", "A MAP NEEDS EXACTLY\nONE ENTRY", twoEntries});

    Map twoExits = makeValidExample();
    twoExits.setCell(0, 6, CellType::Exit);
    examples.push_back({"TWO EXITS", "A MAP NEEDS EXACTLY\nONE EXIT", twoExits});

    Map gap = makeValidExample();
    gap.setCell(3, 4, CellType::Scenery);
    examples.push_back({"GAP IN PATH",
                        "PATH CELLS MUST CONNECT\nTHE ENTRY TO THE EXIT",
                        gap});

    // Path cells that only touch at a corner: critters move up, down, left
    // or right, so these cells are not connected.
    Map diagonal(5, 7);
    diagonal.setCell(1, 1, CellType::Entry);
    diagonal.setCell(2, 2, CellType::Path);
    diagonal.setCell(3, 3, CellType::Path);
    diagonal.setCell(3, 4, CellType::Exit);
    examples.push_back({"DIAGONAL ONLY",
                        "CELLS TOUCHING ONLY AT\nA CORNER ARE NOT\nCONNECTED",
                        diagonal});

    examples.push_back({"BLANK MAP",
                        "A NEW MAP IS ALL SCENERY:\nNO ENTRY AND NO EXIT YET",
                        Map(5, 7)});

    return examples;
}
