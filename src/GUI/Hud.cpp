/**
 * @file Hud.cpp
 * @brief Implements start menu and play sidebar (bitmap text).
 */
#include "Hud.h"
#include "SimpleText.h"

#include <algorithm>

namespace {

bool contains(const sf::FloatRect& r, sf::Vector2f p) {
    return r.width > 0.f && r.height > 0.f && r.contains(p);
}

sf::FloatRect makeRect(float x, float y, float w, float h) {
    return sf::FloatRect(x, y, w, h);
}

} // namespace

void Hud::setMapSize(int rows, int cols) {
    mapRows = rows;
    mapCols = cols;
}

int Hud::getMapRows() const { return mapRows; }
int Hud::getMapCols() const { return mapCols; }

void Hud::setGold(int g) { gold = g; }
int Hud::getGold() const { return gold; }

ShopSelection Hud::getShopSelection() const { return shopSelection; }
void Hud::clearShopSelection() { shopSelection = ShopSelection::None; }

bool Hud::wantsFullscreen() const { return fullscreenPreferred; }
void Hud::setWantsFullscreen(bool on) { fullscreenPreferred = on; }

int Hud::priceOf(ShopSelection item) {
    switch (item) {
        case ShopSelection::Direct: return 100;
        case ShopSelection::Area:   return 150;
        case ShopSelection::Slow:   return 120;
        default: return 0;
    }
}

void Hud::drawLabel(sf::RenderWindow& window, const std::string& text,
                    float x, float y, float scale, sf::Color color) const {
    SimpleText::draw(window, text, x, y, scale, color);
}

void Hud::drawButton(sf::RenderWindow& window, sf::FloatRect rect,
                     const std::string& label, sf::Color fill, bool active) const {
    sf::RectangleShape box(sf::Vector2f(rect.width, rect.height));
    box.setPosition(rect.left, rect.top);
    box.setFillColor(fill);
    box.setOutlineThickness(active ? 3.f : 1.5f);
    box.setOutlineColor(active ? sf::Color(255, 220, 80) : sf::Color(30, 35, 45));
    window.draw(box);

    const float scale = 2.f;
    const float textW = static_cast<float>(label.size()) * 6.f * scale;
    const float textH = 7.f * scale;
    const float tx = rect.left + (rect.width - textW) * 0.5f;
    const float ty = rect.top + (rect.height - textH) * 0.5f;
    SimpleText::draw(window, label, tx, ty, scale, sf::Color::White);
}

void Hud::drawStartMenu(sf::RenderWindow& window) const {
    const float w = static_cast<float>(window.getSize().x);
    const float h = static_cast<float>(window.getSize().y);

    sf::RectangleShape bg(sf::Vector2f(w, h));
    bg.setFillColor(sf::Color(18, 24, 32));
    window.draw(bg);

    sf::RectangleShape panel(sf::Vector2f(480.f, 420.f));
    panel.setPosition((w - 480.f) * 0.5f, (h - 420.f) * 0.5f);
    panel.setFillColor(sf::Color(28, 38, 52));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(70, 110, 150));
    window.draw(panel);

    const float px = panel.getPosition().x;
    const float py = panel.getPosition().y;

    drawLabel(window, "TOWER DEFENSE", px + 80.f, py + 28.f, 3.f, sf::Color(220, 235, 255));
    drawLabel(window, "CHOOSE MAP SIZE THEN START", px + 65.f, py + 70.f, 2.f,
              sf::Color(160, 175, 195));

    drawLabel(window, "ROWS", px + 40.f, py + 125.f, 2.f);
    rowsMinus = makeRect(px + 210.f, py + 115.f, 44.f, 40.f);
    rowsPlus = makeRect(px + 350.f, py + 115.f, 44.f, 40.f);
    drawButton(window, rowsMinus, "-", sf::Color(50, 70, 95));
    drawButton(window, rowsPlus, "+", sf::Color(50, 70, 95));
    drawLabel(window, std::to_string(mapRows), px + 280.f, py + 125.f, 2.5f,
              sf::Color(255, 220, 100));

    drawLabel(window, "COLUMNS", px + 40.f, py + 185.f, 2.f);
    colsMinus = makeRect(px + 210.f, py + 175.f, 44.f, 40.f);
    colsPlus = makeRect(px + 350.f, py + 175.f, 44.f, 40.f);
    drawButton(window, colsMinus, "-", sf::Color(50, 70, 95));
    drawButton(window, colsPlus, "+", sf::Color(50, 70, 95));
    drawLabel(window, std::to_string(mapCols), px + 280.f, py + 185.f, 2.5f,
              sf::Color(255, 220, 100));

    fullscreenBtn = makeRect(px + 80.f, py + 245.f, 320.f, 44.f);
    drawButton(window, fullscreenBtn,
               fullscreenPreferred ? "FULLSCREEN ON" : "FULLSCREEN OFF",
               sf::Color(50, 70, 95), fullscreenPreferred);
    drawLabel(window, "OR PRESS F11 ANYTIME", px + 120.f, py + 300.f, 1.6f,
              sf::Color(120, 140, 160));

    startBtn = makeRect(px + 90.f, py + 340.f, 300.f, 52.f);
    drawButton(window, startBtn, "START GAME", sf::Color(40, 130, 90));
}

bool Hud::handleStartClick(sf::Vector2f pos) {
    if (contains(rowsMinus, pos)) {
        mapRows = std::max(4, mapRows - 1);
        return false;
    }
    if (contains(rowsPlus, pos)) {
        mapRows = std::min(24, mapRows + 1);
        return false;
    }
    if (contains(colsMinus, pos)) {
        mapCols = std::max(4, mapCols - 1);
        return false;
    }
    if (contains(colsPlus, pos)) {
        mapCols = std::min(30, mapCols + 1);
        return false;
    }
    if (contains(fullscreenBtn, pos)) {
        fullscreenPreferred = !fullscreenPreferred;
        return false;
    }
    return contains(startBtn, pos);
}

void Hud::drawPlayHud(sf::RenderWindow& window, const Tower* selected) const {
    const float w = static_cast<float>(window.getSize().x);
    const float h = static_cast<float>(window.getSize().y);
    const float x = w - kSidebarWidth;

    sf::RectangleShape side(sf::Vector2f(kSidebarWidth, h));
    side.setPosition(x, 0.f);
    side.setFillColor(sf::Color(22, 28, 38));
    window.draw(side);

    sf::RectangleShape div(sf::Vector2f(2.f, h));
    div.setPosition(x, 0.f);
    div.setFillColor(sf::Color(55, 75, 100));
    window.draw(div);

    float y = 18.f;
    drawLabel(window, "GOLD", x + 18.f, y, 2.f, sf::Color(160, 175, 195));
    drawLabel(window, "$" + std::to_string(gold), x + 18.f, y + 24.f, 3.f,
              sf::Color(255, 210, 70));
    y += 70.f;

    drawLabel(window, "SHOP", x + 18.f, y, 2.f, sf::Color(160, 175, 195));
    y += 22.f;

    shopDirect = makeRect(x + 14.f, y, kSidebarWidth - 28.f, 44.f);
    drawButton(window, shopDirect,
               "DIRECT $" + std::to_string(priceOf(ShopSelection::Direct)),
               sf::Color(55, 58, 68), shopSelection == ShopSelection::Direct);
    y += 52.f;

    shopArea = makeRect(x + 14.f, y, kSidebarWidth - 28.f, 44.f);
    drawButton(window, shopArea,
               "AREA $" + std::to_string(priceOf(ShopSelection::Area)),
               sf::Color(150, 75, 20), shopSelection == ShopSelection::Area);
    y += 52.f;

    shopSlow = makeRect(x + 14.f, y, kSidebarWidth - 28.f, 44.f);
    drawButton(window, shopSlow,
               "SLOW $" + std::to_string(priceOf(ShopSelection::Slow)),
               sf::Color(40, 110, 160), shopSelection == ShopSelection::Slow);
    y += 60.f;

    if (selected != nullptr) {
        drawLabel(window,
                  selected->getTypeName() + " LV" + std::to_string(selected->getLevel()),
                  x + 18.f, y, 2.f, sf::Color::White);
        y += 24.f;
        drawLabel(window,
                  "RNG " + std::to_string(static_cast<int>(selected->getRange() * 10) / 10) +
                      "  POW " + std::to_string(selected->getPower()),
                  x + 18.f, y, 1.7f, sf::Color(180, 195, 210));
        y += 28.f;

        const bool canUp = selected->canUpgrade();
        upgradeBtn = makeRect(x + 14.f, y, kSidebarWidth - 28.f, 40.f);
        drawButton(window, upgradeBtn,
                   canUp ? ("UPGRADE $" + std::to_string(selected->getUpgradeCost()))
                         : "MAX LEVEL",
                   canUp ? sf::Color(50, 120, 80) : sf::Color(60, 65, 75));
        y += 48.f;

        sellBtn = makeRect(x + 14.f, y, kSidebarWidth - 28.f, 40.f);
        drawButton(window, sellBtn,
                   "SELL +$" + std::to_string(selected->getRefundValue()),
                   sf::Color(140, 55, 55));
        y += 54.f;
    } else {
        upgradeBtn = sf::FloatRect();
        sellBtn = sf::FloatRect();
    }

    drawLabel(window, "ACTIONS", x + 18.f, y, 2.f, sf::Color(160, 175, 195));
    y += 24.f;
    newMapBtn = makeRect(x + 14.f, y, kSidebarWidth - 28.f, 40.f);
    drawButton(window, newMapBtn, "NEW MAP", sf::Color(50, 70, 95));
    y += 48.f;
    playFullscreenBtn = makeRect(x + 14.f, y, kSidebarWidth - 28.f, 40.f);
    drawButton(window, playFullscreenBtn,
               fullscreenPreferred ? "WINDOWED" : "FULLSCREEN",
               sf::Color(50, 70, 95));
    y += 36.f;
}

std::string Hud::handlePlayClick(sf::Vector2f pos, const Tower* selected) {
    if (contains(shopDirect, pos)) {
        shopSelection = (shopSelection == ShopSelection::Direct)
                            ? ShopSelection::None
                            : ShopSelection::Direct;
        return "buy_direct";
    }
    if (contains(shopArea, pos)) {
        shopSelection = (shopSelection == ShopSelection::Area)
                            ? ShopSelection::None
                            : ShopSelection::Area;
        return "buy_area";
    }
    if (contains(shopSlow, pos)) {
        shopSelection = (shopSelection == ShopSelection::Slow)
                            ? ShopSelection::None
                            : ShopSelection::Slow;
        return "buy_slow";
    }
    if (selected != nullptr && contains(upgradeBtn, pos)) {
        return "upgrade";
    }
    if (selected != nullptr && contains(sellBtn, pos)) {
        return "sell";
    }
    if (contains(newMapBtn, pos)) {
        return "new_map";
    }
    if (contains(playFullscreenBtn, pos)) {
        return "toggle_fullscreen";
    }
    return "none";
}
