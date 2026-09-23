/**
 * @file Map.h
 * @brief Declares the Map class and its cell types for the tower defense game.
 *
 * Game rules for the map:
 * - The map is a grid of cells. Its size is chosen before it is created.
 * - Scenery cells: towers can be placed; critters cannot walk.
 * - Path cells: critters walk; towers cannot be placed.
 * - Exactly one Entry cell and exactly one Exit cell.
 * - Path cells must connect the Entry to the Exit.
 */
#ifndef Map_h
#define Map_h

#include <vector>

/** @brief What a single cell on the map contains. */
enum class CellType {
    Scenery, ///< Towers may be placed here; critters cannot walk here.
    Path,    ///< Critters walk here; towers cannot be placed here.
    Entry,   ///< The single path cell where critters appear.
    Exit     ///< The single path cell critters try to reach.
};

/** @brief A row/column position on the map. */
struct Position {
    int row; ///< Row index (0 is the top row).
    int col; ///< Column index (0 is the leftmost column).
};

/**
 * @brief A grid map made of scenery and path cells.
 *
 * A new map is blank: every cell starts as Scenery. Use setCell() to draw
 * the path, entry and exit, then call isValid() to check that the map
 * follows the rules. Cells are addressed as (row, col), where row is in
 * [0, getLength()) and col is in [0, getWidth()).
 */
class Map {
public:

// --- Construction ---
    /**
     * @brief Creates a blank map where every cell is Scenery.
     * @param length Number of rows. Must be greater than 0.
     * @param width  Number of columns. Must be greater than 0.
     */
    Map(int length, int width);

// --- Getters ---
    /** @return Number of rows in the map. */
    int getLength() const;

    /** @return Number of columns in the map. */
    int getWidth() const;

    /**
     * @brief Returns the type of the cell at (row, col).
     * @param row Row index of the cell.
     * @param col Column index of the cell.
     * @return The cell's type.
     * @pre isInBounds(row, col) is true.
     */
    CellType getCellType(int row, int col) const;

// --- Setters ---
    /**
     * @brief Sets the cell at (row, col) to the given type.
     *
     * Any cell can be set to any type. This function does not enforce the
     * one-entry/one-exit rule, so a map may be temporarily invalid while it
     * is being built; use isValid() to check it afterwards.
     * @param row  Row index of the cell.
     * @param col  Column index of the cell.
     * @param type The new type for the cell.
     * @return true if the cell was set, false if (row, col) is out of bounds.
     */
    bool setCell(int row, int col, CellType type);

// --- Queries used by towers and critters ---
    /**
     * @brief Checks whether a position is inside the map.
     * @param row Row index to check.
     * @param col Column index to check.
     * @return true if 0 <= row < getLength() and 0 <= col < getWidth().
     */
    bool isInBounds(int row, int col) const;

    /**
     * @brief Checks whether a tower may be placed at (row, col).
     * @param row Row index of the cell.
     * @param col Column index of the cell.
     * @return true if the cell is in bounds and is Scenery.
     */
    bool canPlaceTower(int row, int col) const;

    /**
     * @brief Checks whether a critter may walk on (row, col).
     * @param row Row index of the cell.
     * @param col Column index of the cell.
     * @return true if the cell is in bounds and is Path, Entry or Exit.
     */
    bool isWalkable(int row, int col) const;

// --- Validation ---
    /**
     * @brief Checks that the map follows all the map rules.
     *
     * A valid map has exactly one Entry cell, exactly one Exit cell, and a
     * connected route of walkable cells (moving up, down, left or right)
     * from the Entry to the Exit.
     * @return true if the map is valid.
     */
    bool isValid() const;

    /**
     * @brief Finds the route critters follow from the Entry to the Exit.
     *
     * Uses a breadth-first search over walkable cells, so the route
     * returned is a shortest one.
     * @return The cells in order from Entry to Exit, or an empty vector
     *         if there is no Entry, no Exit, or no route between them.
     */
    std::vector<Position> findPath() const;

private:
    int length;                              ///< Number of rows.
    int width;                               ///< Number of columns.
    std::vector<std::vector<CellType>> grid; ///< Cells, accessed as grid[row][col].
};

#endif // Map_h
