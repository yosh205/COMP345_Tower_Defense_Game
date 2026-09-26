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
    None,
    Direct,
    Area,
    Slow
};

/**
 * @brief All on-screen menus: map-size start screen and play HUD.
 *
 * Layout while playing: map on the left, fixed-width sidebar on the right
 * with gold, shop buttons, selected-tower upgrade/sell, and wave actions.
 */
class Hud {
public:
    static constexpr float kSidebarWidth = 280.f;

    Hud() = default;

    // --- Start menu ---
    void setMapSize(int rows, int cols);
    int getMapRows() const;
    int getMapCols() const;
    void drawStartMenu(sf::RenderWindow& window) const;

    /**
     * @brief Handles a click on the start menu.
     * @return true if the player pressed Start.
     */
    bool handleStartClick(sf::Vector2f pos);

    // --- Play HUD ---
    void setGold(int gold);
    int getGold() const;
    ShopSelection getShopSelection() const;
    void clearShopSelection();

    void drawPlayHud(sf::RenderWindow& window, const Tower* selected) const;

    /**
     * @brief Handles a click inside the sidebar during play.
     * @return Action code: "none", "buy_direct", "buy_area", "buy_slow",
     *         "upgrade", "sell", "new_map", "toggle_fullscreen".
     */
    std::string handlePlayClick(sf::Vector2f pos, const Tower* selected);

    /** @return Shop price for a tower type (matches tower buy costs). */
    static int priceOf(ShopSelection item);

    /** @brief True when the start-menu fullscreen checkbox is on. */
    bool wantsFullscreen() const;
    void setWantsFullscreen(bool on);

private:
    int mapRows = 10;
    int mapCols = 12;
    int gold = 500;
    ShopSelection shopSelection = ShopSelection::None;
    bool fullscreenPreferred = false;

    void drawButton(sf::RenderWindow& window, sf::FloatRect rect,
                    const std::string& label, sf::Color fill,
                    bool active = false) const;
    void drawLabel(sf::RenderWindow& window, const std::string& text,
                   float x, float y, float scale,
                   sf::Color color = sf::Color::White) const;

    mutable sf::FloatRect startBtn;
    mutable sf::FloatRect rowsMinus, rowsPlus;
    mutable sf::FloatRect colsMinus, colsPlus;
    mutable sf::FloatRect fullscreenBtn;

    mutable sf::FloatRect shopDirect, shopArea, shopSlow;
    mutable sf::FloatRect upgradeBtn, sellBtn, newMapBtn, playFullscreenBtn;
};

#endif // Hud_h
