/**
 * @file Hud.h
 * @brief Start menu + in-game shop / tower inspector sidebar.
 */
#ifndef Hud_h
#define Hud_h

#include <SFML/Graphics.hpp>
#include <string>

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
     * @brief Draws the start menu: title, rows/columns selectors, fullscreen option and Start button.
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
     * @brief Checks whether a start-menu click hit the Map Rules Demo button.
     * @param pos Mouse position in window pixels.
     * @return true if the player clicked Map Rules Demo.
     */
    bool clickedMapDemo(sf::Vector2f pos) const;

    /**
     * @brief Checks whether a start-menu click hit the Map Observer Demo button.
     * @param pos Mouse position in window pixels.
     * @return true if the player clicked Map Observer Demo.
     */
    bool clickedMapObserverDemo(sf::Vector2f pos) const;

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
     * @brief Draws the sidebar: gold, wave, shop, selected tower (stats, Upgrade, Sell) and actions.
     * @param window   The window to draw into.
     * @param selected The selected tower, or nullptr to hide the tower section.
     */
    void drawPlayHud(sf::RenderWindow& window, const Tower* selected) const;

    /**
     * @brief Handles a click inside the sidebar during play.
     * @param pos      Mouse position in window pixels.
     * @param selected The selected tower, or nullptr (Upgrade/Sell only work with one).
     * @return Action code: "none", "buy_direct", "buy_area", "buy_slow",
     *         "upgrade", "sell", "start_wave", "new_map", "toggle_fullscreen".
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
    int gold = 500;               ///< Player's gold shown in the sidebar.
    int healthHalves = kHeartsPerWave * 2;  ///< Health left, in half-hearts. 6 = full.
    ShopSelection shopSelection = ShopSelection::None;  ///< Tower type armed for placement.
    bool fullscreenPreferred = false;  ///< Fullscreen option from the menu / F11.
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
    mutable sf::FloatRect rowsMinus;          ///< Start menu: rows "-" button.
    mutable sf::FloatRect rowsPlus;           ///< Start menu: rows "+" button.
    mutable sf::FloatRect colsMinus;          ///< Start menu: columns "-" button.
    mutable sf::FloatRect colsPlus;           ///< Start menu: columns "+" button.
    mutable sf::FloatRect fullscreenBtn;      ///< Start menu: fullscreen on/off button.
    mutable sf::FloatRect mapDemoBtn;         ///< Start menu: Map Rules Demo button.
    mutable sf::FloatRect mapObserverDemoBtn; ///< Start menu: Map Observer Demo button.

    mutable sf::FloatRect demoPrevBtn;        ///< Map demo: previous example.
    mutable sf::FloatRect demoNextBtn;        ///< Map demo: next example.
    mutable sf::FloatRect demoBackBtn;        ///< Map demo: back to the start menu.

    mutable sf::FloatRect shopDirect;         ///< Sidebar: buy Direct tower.
    mutable sf::FloatRect shopArea;           ///< Sidebar: buy Area tower.
    mutable sf::FloatRect shopSlow;           ///< Sidebar: buy Slow tower.
    mutable sf::FloatRect upgradeBtn;         ///< Sidebar: upgrade the selected tower.
    mutable sf::FloatRect sellBtn;            ///< Sidebar: sell the selected tower.
    mutable sf::FloatRect newMapBtn;          ///< Sidebar: generate a new map.
    mutable sf::FloatRect playFullscreenBtn;  ///< Sidebar: toggle fullscreen.
    mutable sf::FloatRect startWaveBtn;       ///< Sidebar: Start Wave (empty while a wave runs).

    mutable sf::FloatRect gameOverAgainBtn;  ///< Game over: Play Again.
    mutable sf::FloatRect gameOverMenuBtn;   ///< Game over: Main Menu.
};

#endif // Hud_h
