/**
 * @file Hud.h
 * @brief Start menu + in-game shop / tower inspector sidebar.
 */
#ifndef Hud_h
#define Hud_h

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "../Map/MapEditor.h"
#include "../Tower/Tower.h"

/** @brief Which shop item is armed for placement (or none). */
enum class ShopSelection {
    None,    ///< Nothing selected: clicking the map selects a tower.
    Direct,  ///< Next map click places a DirectDamageTower.
    Area,    ///< Next map click places an AreaDamageTower.
    Slow     ///< Next map click places a SlowingTower.
};

/**
 * @brief All on-screen menus: map-size start screen and play HUD.
 *
 * Layout while playing: map on the left, fixed-width sidebar on the right
 * with gold, shop buttons, selected-tower upgrade/sell, and wave actions.
 *
 * Hud only draws and reports clicks. It does not change towers or critters
 * itself: handlePlayClick() returns an action code and main.cpp applies it.
 */
class Hud {
public:
    /** @brief Width of the sidebar on the right of the window, in pixels. */
    static constexpr float kSidebarWidth = 280.f;

    /** @brief Hearts the player starts each wave with. One escape removes half a heart. */
    static constexpr int kHeartsPerWave = 3;

    /** @brief Creates the HUD with a 10 x 12 map size, 500 gold and nothing selected. */
    Hud() = default;

    // --- Start menu ---
    /**
     * @brief Sets the map size shown in the start menu.
     * @param rows Number of rows.
     * @param cols Number of columns.
     */
    void setMapSize(int rows, int cols);
    /** @return Number of map rows chosen in the start menu (4 to 24). */
    int getMapRows() const;
    /** @return Number of map columns chosen in the start menu (4 to 30). */
    int getMapCols() const;

    /**
     * @brief Sets the maps the start menu offers. The menu always offers RANDOM
     * first (a random map of the chosen rows and columns), then these.
     * @param names Names of the hand-built maps, in order.
     */
    void setMapChoices(const std::vector<std::string>& names);

    /**
     * @return 0 for RANDOM, otherwise the chosen hand-built map plus one
     *         (1 = the first name given to setMapChoices()).
     */
    int getMapChoice() const;

    /**
     * @brief Selects a map in the start menu, e.g. one just saved in the map editor.
     * @param choice 0 for RANDOM, otherwise a hand-built map plus one. Ignored if out of range.
     */
    void setMapChoice(int choice);
    /**
     * @brief Draws the start menu: title, map selector, rows/columns selectors (random map only),
     * fullscreen option, Start button and the demo buttons.
     * @param window The window to draw into.
     */
    void drawStartMenu(sf::RenderWindow& window) const;

    /**
     * @brief Handles a click on the start menu.
     * @param pos Mouse position in window pixels.
     * @return true if the player pressed Start.
     */
    bool handleStartClick(sf::Vector2f pos);

    /**
     * @brief Checks whether a start-menu click hit the Demos button.
     * @param pos Mouse position in window pixels.
     * @return true if the player clicked Demos.
     */
    bool clickedDemos(sf::Vector2f pos) const;

    /**
     * @brief Checks whether a start-menu click hit the Map Editor button.
     * @param pos Mouse position in window pixels.
     * @return true if the player clicked Map Editor.
     */
    bool clickedMapEditor(sf::Vector2f pos) const;

    // --- Map editor ---
    /**
     * @brief Draws the map editor sidebar: tools, whether the map is valid (or
     * what is missing), and Clear / Save / Play / Back buttons.
     * @param window  The window to draw into.
     * @param tool    The selected tool (highlighted).
     * @param valid   Result of Map::isValid() for the map being drawn.
     * @param problem What is missing when the map is not valid (lines separated by '\n').
     * @param unused  Path cells not on the route (shown as a note when not 0).
     * @param message Result of the last action, e.g. "SAVED AS CUSTOM 1", or "".
     */
    void drawMapEditor(sf::RenderWindow& window, EditorTool tool, bool valid,
                       const std::string& problem, int unused,
                       const std::string& message) const;

    /**
     * @brief Handles a click in the map editor sidebar.
     * @param pos Mouse position in window pixels.
     * @return "tool_path", "tool_entry", "tool_exit", "tool_erase", "clear",
     *         "save", "play", "back" or "none".
     */
    std::string handleMapEditorClick(sf::Vector2f pos) const;

    // --- Demos screen ---
    /**
     * @brief Draws the demos screen: one button per assignment driver
     * (Assignment 2: Observer; Assignment 3: Strategy, Factory, Decorator) and Back.
     * @param window The window to draw into.
     */
    void drawDemos(sf::RenderWindow& window) const;

    /**
     * @brief Handles a click on the demos screen.
     * @param pos Mouse position in window pixels.
     * @return "map_rules", "map_observer", "critter_observer", "tower_strategy",
     *         "critter_factory", "tower_decorator", "back" or "none".
     */
    std::string handleDemosClick(sf::Vector2f pos) const;

    // --- Map rules demo ---
    /**
     * @brief Draws the demo sidebar: the example's name, the result of
     * Map::isValid() (VALID / INVALID), the rule it shows, and Prev / Next / Back buttons.
     * @param window The window to draw into.
     * @param name   Short name of the example map.
     * @param rule   The map rule the example shows (lines separated by '\n').
     * @param valid  Result of Map::isValid() for the example map.
     * @param index  Position of the example in the list (0 = first).
     * @param count  Number of examples.
     */
    void drawMapDemo(sf::RenderWindow& window, const std::string& name,
                     const std::string& rule, bool valid, int index, int count) const;

    /**
     * @brief Handles a click in the demo sidebar.
     * @param pos Mouse position in window pixels.
     * @return "prev", "next", "back" or "none".
     */
    std::string handleMapDemoClick(sf::Vector2f pos) const;

    // --- Play HUD ---
    /**
     * @brief Sets the player's gold shown in the sidebar.
     * @param gold New amount of gold.
     */
    void setGold(int gold);
    /** @return The player's current gold. */
    int getGold() const;

    /**
     * @brief Fills the health bar back to kHeartsPerWave hearts.
     * Called at the start of each wave (and when a new game or map begins).
     */
    void refillHealth();

    /**
     * @brief Removes half a heart for each critter that reached the exit.
     * Health never goes below 0.
     * @param escapes Number of critters that escaped this frame.
     */
    void loseHalfHearts(int escapes);

    /** @return Health left, in half-hearts (6 = 3 hearts, 1 = half a heart). */
    int getHealthHalves() const;

    /** @return True when no hearts are left and the run is over. */
    bool isDefeated() const;
    /** @return The tower type armed for placement, or ShopSelection::None. */
    ShopSelection getShopSelection() const;
    /** @brief Disarms the shop (ShopSelection::None), e.g. after placing a tower. */
    void clearShopSelection();

    /**
     * @brief Updates the wave display in the sidebar.
     * @param wave         Current wave number (0 before the first wave).
     * @param crittersLeft Critters of the wave not yet killed or escaped.
     * @param inProgress   True while a wave runs; hides the Start Wave button.
     */
    void setWaveInfo(int wave, int crittersLeft, bool inProgress);

    /**
     * @brief Sets the targeting strategy name shown on the selected tower's TARGET button.
     * @param name Short name in capitals, e.g. "NEAREST EXIT".
     */
    void setTargetingName(const std::string& name);

    /**
     * @brief Draws the sidebar: gold, wave, shop, selected tower (stats, targeting,
     * special effects, Upgrade, Sell) and actions.
     * @param window   The window to draw into.
     * @param selected The selected tower, or nullptr to hide the tower section.
     */
    void drawPlayHud(sf::RenderWindow& window, const Tower* selected) const;

    /**
     * @brief Handles a click inside the sidebar during play.
     * @param pos      Mouse position in window pixels.
     * @param selected The selected tower, or nullptr (Upgrade/Sell only work with one).
     * @return Action code: "none", "buy_direct", "buy_area", "buy_slow",
     *         "upgrade", "sell", "start_wave", "new_map", "toggle_fullscreen",
     *         "toggle_overlap", "cycle_strategy" (TARGET button), "add_splash",
     *         "add_burning", "add_freezing".
     *         "new_map" means a new random map, or restarting the chosen hand-built map.
     */
    std::string handlePlayClick(sf::Vector2f pos, const Tower* selected);

    /**
     * @brief Draws the game over screen in the same style as the start menu.
     * @param window The window to draw into.
     * @param wave   The wave the player lost on.
     */
    void drawGameOver(sf::RenderWindow& window, int wave) const;

    /**
     * @brief Handles a click on the game over screen.
     * @param pos Mouse position in window pixels.
     * @return "again", "menu" or "none".
     */
    std::string handleGameOverClick(sf::Vector2f pos) const;

    /**
     * @param item A tower type.
     * @return Shop price for a tower type (matches tower buy costs).
     */
    static int priceOf(ShopSelection item);

    /**
     * @return True when critters may stand on the same path tile.
     * The start menu and the play sidebar both change this. Default is false
     * (one critter per tile, as the project specification asks).
     */
    bool crittersCanOverlap() const;
    /**
     * @brief Sets whether critters may share a path tile.
     * @param allow True to allow overlap, false to keep one critter per tile.
     */
    void setCrittersCanOverlap(bool allow);

    /** @return True when the start-menu fullscreen checkbox is on. */
    bool wantsFullscreen() const;
    /**
     * @brief Sets the fullscreen option (kept in sync when F11 toggles fullscreen).
     * @param on True for fullscreen.
     */
    void setWantsFullscreen(bool on);

private:
    int mapRows = 10;             ///< Map rows chosen in the start menu.
    int mapCols = 12;             ///< Map columns chosen in the start menu.
    std::vector<std::string> mapChoices{"RANDOM"};  ///< Start menu map names; RANDOM first.
    int mapChoice = 0;            ///< Index into mapChoices.
    int gold = 500;               ///< Player's gold shown in the sidebar.
    int healthHalves = kHeartsPerWave * 2;  ///< Health left, in half-hearts. 6 = full.
    ShopSelection shopSelection = ShopSelection::None;  ///< Tower type armed for placement.
    bool fullscreenPreferred = false;  ///< Fullscreen option from the menu / F11.
    bool critterOverlap = false;       ///< True: critters may share a path tile.
    std::string targetingName = "NEAREST EXIT";  ///< Selected tower's strategy, shown on TARGET.
    int wave = 0;                 ///< Wave number shown in the sidebar.
    int crittersLeft = 0;         ///< Critters of the wave still in play.
    bool waveInProgress = false;  ///< True while a wave runs.

    /**
     * @brief Draws a filled button with centred text.
     * @param window The window to draw into.
     * @param rect   Button area, in pixels.
     * @param label  Text on the button.
     * @param fill   Background colour.
     * @param active True to draw a highlighted (yellow) outline.
     */
    void drawButton(sf::RenderWindow& window, sf::FloatRect rect,
                    const std::string& label, sf::Color fill,
                    bool active = false) const;
    /**
     * @brief Draws text with SimpleText.
     * @param window The window to draw into.
     * @param text   Text to draw.
     * @param x      Left edge, in pixels.
     * @param y      Top edge, in pixels.
     * @param scale  Size of one font pixel.
     * @param color  Text colour.
     */
    void drawLabel(sf::RenderWindow& window, const std::string& text,
                   float x, float y, float scale,
                   sf::Color color = sf::Color::White) const;

    /**
     * @brief Draws one pixel heart, full, half or empty, in the bitmap-font style.
     * @param window The window to draw into.
     * @param x      Left edge, in pixels.
     * @param y      Top edge, in pixels.
     * @param scale  Size of one heart pixel (2–3 matches the sidebar text).
     * @param halves 0 empty, 1 left half filled, 2 full.
     */
    void drawHeart(sf::RenderWindow& window, float x, float y, float scale, int halves) const;

    // Button areas, in pixels. They are set while drawing (so they are
    // 'mutable' in the const draw functions) and read when handling clicks.
    // An empty rectangle means the button is hidden and cannot be clicked.

    mutable sf::FloatRect startBtn;           ///< Start menu: Start Game button.
    mutable sf::FloatRect mapPrevBtn;         ///< Start menu: previous map "<" button.
    mutable sf::FloatRect mapNextBtn;         ///< Start menu: next map ">" button.
    mutable sf::FloatRect rowsMinus;          ///< Start menu: rows "-" button.
    mutable sf::FloatRect rowsPlus;           ///< Start menu: rows "+" button.
    mutable sf::FloatRect colsMinus;          ///< Start menu: columns "-" button.
    mutable sf::FloatRect colsPlus;           ///< Start menu: columns "+" button.
    mutable sf::FloatRect fullscreenBtn;      ///< Start menu: fullscreen on/off button.
    mutable sf::FloatRect overlapBtn;         ///< Start menu: critter overlap on/off button.
    mutable sf::FloatRect mapEditorBtn;       ///< Start menu: Map Editor button.
    mutable sf::FloatRect demosBtn;           ///< Start menu: Demos button.

    mutable sf::FloatRect demoMapRulesBtn;        ///< Demos: Map Rules (Assignment 1).
    mutable sf::FloatRect demoMapObserverBtn;     ///< Demos: Map Observer (Assignment 2).
    mutable sf::FloatRect demoCritterObserverBtn; ///< Demos: Critter Observer (Assignment 2).
    mutable sf::FloatRect demoStrategyBtn;        ///< Demos: Tower Strategy (Assignment 3).
    mutable sf::FloatRect demoFactoryBtn;         ///< Demos: Critter Factory (Assignment 3).
    mutable sf::FloatRect demoDecoratorBtn;       ///< Demos: Tower Decorator (Assignment 3).
    mutable sf::FloatRect demosBackBtn;           ///< Demos: back to the start menu.

    mutable sf::FloatRect editorPathBtn;      ///< Map editor: Path tool.
    mutable sf::FloatRect editorEntryBtn;     ///< Map editor: Entry tool.
    mutable sf::FloatRect editorExitBtn;      ///< Map editor: Exit tool.
    mutable sf::FloatRect editorEraseBtn;     ///< Map editor: Erase (scenery) tool.
    mutable sf::FloatRect editorClearBtn;     ///< Map editor: clear the whole map.
    mutable sf::FloatRect editorSaveBtn;      ///< Map editor: add the map to the start menu.
    mutable sf::FloatRect editorPlayBtn;      ///< Map editor: play the map.
    mutable sf::FloatRect editorBackBtn;      ///< Map editor: back to the start menu.

    mutable sf::FloatRect demoPrevBtn;        ///< Map demo: previous example.
    mutable sf::FloatRect demoNextBtn;        ///< Map demo: next example.
    mutable sf::FloatRect demoBackBtn;        ///< Map demo: back to the start menu.

    mutable sf::FloatRect shopDirect;         ///< Sidebar: buy Direct tower.
    mutable sf::FloatRect shopArea;           ///< Sidebar: buy Area tower.
    mutable sf::FloatRect shopSlow;           ///< Sidebar: buy Slow tower.
    mutable sf::FloatRect upgradeBtn;         ///< Sidebar: upgrade the selected tower.
    mutable sf::FloatRect sellBtn;            ///< Sidebar: sell the selected tower.
    mutable sf::FloatRect targetingBtn;       ///< Sidebar: change the selected tower's strategy.
    mutable sf::FloatRect splashBtn;          ///< Sidebar: add splash to the selected tower.
    mutable sf::FloatRect burningBtn;         ///< Sidebar: add burning to the selected tower.
    mutable sf::FloatRect freezingBtn;        ///< Sidebar: add freezing to the selected tower.
    mutable sf::FloatRect newMapBtn;          ///< Sidebar: generate a new map.
    mutable sf::FloatRect playFullscreenBtn;  ///< Sidebar: toggle fullscreen.
    mutable sf::FloatRect playOverlapBtn;     ///< Sidebar: toggle critter overlap.
    mutable sf::FloatRect startWaveBtn;       ///< Sidebar: Start Wave (empty while a wave runs).

    mutable sf::FloatRect gameOverAgainBtn;  ///< Game over: Play Again.
    mutable sf::FloatRect gameOverMenuBtn;   ///< Game over: Main Menu.
};

#endif // Hud_h
