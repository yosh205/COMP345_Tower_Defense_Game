/**
 * @file MapEditor.h
 * @brief Declares MapEditor, which lets the player draw their own map cell by cell.
 */
#ifndef MapEditor_h
#define MapEditor_h

#include <string>

#include "Map.h"

/** @brief What a click on a cell does in the map editor. */
enum class EditorTool {
    Path,    ///< Makes the cell a Path cell.
    Entry,   ///< Moves the map's single Entry to the cell.
    Exit,    ///< Moves the map's single Exit to the cell.
    Scenery  ///< Erases the cell back to Scenery.
};

/**
 * @brief Edits a map one cell at a time, following the map rules.
 *
 * The editor keeps at most one Entry and one Exit: placing a new one turns the
 * old one back into Scenery. The map can still be invalid while it is being
 * drawn (no entry yet, path not connected), and getProblem() says what is missing.
 * It contains no graphics code; the game draws getMap() and passes clicks to paint().
 */
class MapEditor {
public:
    /**
     * @brief Starts editing a blank map (all Scenery).
     * @param length Number of rows. Must be greater than 0.
     * @param width  Number of columns. Must be greater than 0.
     */
    MapEditor(int length, int width);

    /**
     * @brief Starts editing a copy of an existing map.
     * @param start The map to edit; it is not changed.
     */
    explicit MapEditor(const Map& start);

    /** @param tool The tool that paint() uses from now on. */
    void setTool(EditorTool tool);
    /** @return The tool that paint() uses. */
    EditorTool getTool() const;

    /**
     * @brief Applies the current tool to a cell.
     * @param row Row of the cell.
     * @param col Column of the cell.
     * @return true if the map changed; false if the cell is outside the map or already had that type.
     */
    bool paint(int row, int col);

    /**
     * @brief Turns a cell back into Scenery, whatever the current tool is.
     * @param row Row of the cell.
     * @param col Column of the cell.
     * @return true if the map changed.
     */
    bool erase(int row, int col);

    /** @brief Turns every cell back into Scenery. The size stays the same. */
    void clear();

    /** @return The map being edited. */
    const Map& getMap() const;

    /** @return true if the map can be played (Map::isValid()). */
    bool isValid() const;

    /**
     * @return What the player still has to do for the map to be valid, in
     *         capitals with '\n' between lines, or "" when the map is valid.
     */
    std::string getProblem() const;

    /**
     * @return Number of path cells critters never walk on (branches or loops
     *         off the route), or 0 when the map is not valid.
     */
    int getUnusedPathCells() const;

private:
    /**
     * @brief Turns every cell of the given type back into Scenery, except (row, col).
     * Used to keep a single Entry and a single Exit.
     */
    void removeOthers(CellType type, int row, int col);

    /** @return Number of cells of the given type. */
    int count(CellType type) const;

    Map map;                            ///< The map being drawn.
    EditorTool tool = EditorTool::Path; ///< What paint() does.
};

#endif // MapEditor_h
