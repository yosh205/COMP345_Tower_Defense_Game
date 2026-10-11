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

    // --- Play HUD ---
    /**
     * @brief Sets the player's gold shown in the sidebar.
     * @param gold New amount of gold.
     */
    void setGold(int gold);
    /** @return The player's current gold. */
    int getGold() const;
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

    // Button areas, in pixels. They are set while drawing (so they are
    // 'mutable' in the const draw functions) and read when handling clicks.
    // An empty rectangle means the button is hidden and cannot be clicked.

    mutable sf::FloatRect startBtn;           ///< Start menu: Start Game button.
    mutable sf::FloatRect rowsMinus;          ///< Start menu: rows "-" button.
    mutable sf::FloatRect rowsPlus;           ///< Start menu: rows "+" button.
    mutable sf::FloatRect colsMinus;          ///< Start menu: columns "-" button.
    mutable sf::FloatRect colsPlus;           ///< Start menu: columns "+" button.
    mutable sf::FloatRect fullscreenBtn;      ///< Start menu: fullscreen on/off button.

    mutable sf::FloatRect shopDirect;         ///< Sidebar: buy Direct tower.
    mutable sf::FloatRect shopArea;           ///< Sidebar: buy Area tower.
    mutable sf::FloatRect shopSlow;           ///< Sidebar: buy Slow tower.
    mutable sf::FloatRect upgradeBtn;         ///< Sidebar: upgrade the selected tower.
    mutable sf::FloatRect sellBtn;            ///< Sidebar: sell the selected tower.
    mutable sf::FloatRect newMapBtn;          ///< Sidebar: generate a new map.
    mutable sf::FloatRect playFullscreenBtn;  ///< Sidebar: toggle fullscreen.
    mutable sf::FloatRect startWaveBtn;       ///< Sidebar: Start Wave (empty while a wave runs).
};

#endif // Hud_h
