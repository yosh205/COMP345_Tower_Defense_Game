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

void Hud::refillHealth() { healthHalves = kHeartsPerWave * 2; }

void Hud::loseHalfHearts(int escapes) {
    if (escapes <= 0) {
        return;
    }
    healthHalves = std::max(0, healthHalves - escapes);
}

int Hud::getHealthHalves() const { return healthHalves; }
bool Hud::isDefeated() const { return healthHalves <= 0; }

ShopSelection Hud::getShopSelection() const { return shopSelection; }
void Hud::clearShopSelection() { shopSelection = ShopSelection::None; }

void Hud::setWaveInfo(int w, int left, bool inProgress) {
    wave = w;
    crittersLeft = left;
    waveInProgress = inProgress;
}

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

void Hud::drawHeart(sf::RenderWindow& window, float x, float y, float size, int halves) const {
    const sf::Color filled(214, 54, 68);
    const sf::Color empty(48, 36, 42);
    const sf::Color line(110, 36, 46);
    const auto lobeColor = [&](bool left) {
        if (halves >= 2) {
            return filled;
        }
        if (halves == 1 && left) {
            return filled;
        }
        return empty;
    };

    // Point of the heart, split so a half heart can fill only the left side.
    sf::ConvexShape leftTip;
    leftTip.setPointCount(3);
    leftTip.setPoint(0, sf::Vector2f(x + size * 0.50f, y + size * 0.36f));
    leftTip.setPoint(1, sf::Vector2f(x + size * 0.08f, y + size * 0.40f));
    leftTip.setPoint(2, sf::Vector2f(x + size * 0.50f, y + size * 0.96f));
    leftTip.setFillColor(halves >= 1 ? filled : empty);

    sf::ConvexShape rightTip;
    rightTip.setPointCount(3);
    rightTip.setPoint(0, sf::Vector2f(x + size * 0.50f, y + size * 0.36f));
    rightTip.setPoint(1, sf::Vector2f(x + size * 0.92f, y + size * 0.40f));
    rightTip.setPoint(2, sf::Vector2f(x + size * 0.50f, y + size * 0.96f));
    rightTip.setFillColor(halves >= 2 ? filled : empty);

    window.draw(leftTip);
    window.draw(rightTip);

    sf::CircleShape leftLobe(size * 0.26f);
    leftLobe.setOrigin(leftLobe.getRadius(), leftLobe.getRadius());
    leftLobe.setPosition(x + size * 0.32f, y + size * 0.32f);
    leftLobe.setFillColor(lobeColor(true));
    leftLobe.setOutlineThickness(1.25f);
    leftLobe.setOutlineColor(line);
    window.draw(leftLobe);

    sf::CircleShape rightLobe(size * 0.26f);
    rightLobe.setOrigin(rightLobe.getRadius(), rightLobe.getRadius());
    rightLobe.setPosition(x + size * 0.68f, y + size * 0.32f);
    rightLobe.setFillColor(lobeColor(false));
    rightLobe.setOutlineThickness(1.25f);
    rightLobe.setOutlineColor(line);
    window.draw(rightLobe);
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

    sf::RectangleShape panel(sf::Vector2f(480.f, 480.f));
    panel.setPosition((w - 480.f) * 0.5f, (h - 480.f) * 0.5f);
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

    // Opens the screen that shows which maps Map::isValid() accepts and rejects.
    mapDemoBtn = makeRect(px + 90.f, py + 410.f, 300.f, 44.f);
    drawButton(window, mapDemoBtn, "MAP RULES DEMO", sf::Color(50, 70, 95));
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
    // Wave number, to the right of the gold.
    drawLabel(window, "WAVE", x + 160.f, y, 2.f, sf::Color(160, 175, 195));
    drawLabel(window, std::to_string(wave), x + 160.f, y + 24.f, 3.f,
              sf::Color(230, 120, 230));
    y += 64.f;

    drawLabel(window, "HEALTH", x + 18.f, y, 2.f, sf::Color(160, 175, 195));
    const float heartSize = 28.f;
    const float heartGap = 10.f;
    const float heartsX = x + 18.f;
    const float heartsY = y + 24.f;

    sf::RectangleShape healthBar(sf::Vector2f(kSidebarWidth - 36.f, heartSize + 12.f));
    healthBar.setPosition(x + 14.f, heartsY - 4.f);
    healthBar.setFillColor(sf::Color(14, 16, 22));
    healthBar.setOutlineThickness(1.5f);
    healthBar.setOutlineColor(sf::Color(70, 110, 150));
    window.draw(healthBar);

    for (int i = 0; i < kHeartsPerWave; ++i) {
        const int have = healthHalves - i * 2;
        const int fill = have >= 2 ? 2 : (have == 1 ? 1 : 0);
        drawHeart(window, heartsX + i * (heartSize + heartGap), heartsY + 2.f, heartSize, fill);
    }
    const int wholeHearts = healthHalves / 2;
    const std::string healthText = (healthHalves % 2 == 0)
                                        ? std::to_string(wholeHearts)
                                        : (std::to_string(wholeHearts) + ".5");
    drawLabel(window, healthText,
              heartsX + kHeartsPerWave * (heartSize + heartGap) + 8.f,
              heartsY + 8.f, 2.f, sf::Color(235, 90, 100));
    y += 62.f;

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
    if (waveInProgress) {
        // While a wave runs there is no button (an empty rect cannot be
        // clicked); show how many critters are still in play instead.
        startWaveBtn = sf::FloatRect();
        drawLabel(window, "CRITTERS LEFT: " + std::to_string(crittersLeft),
                  x + 18.f, y + 12.f, 2.f, sf::Color(230, 120, 230));
    } else {
        startWaveBtn = makeRect(x + 14.f, y, kSidebarWidth - 28.f, 40.f);
        drawButton(window, startWaveBtn, "START WAVE " + std::to_string(wave + 1),
                   sf::Color(120, 50, 140));
    }
    y += 48.f;
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
    if (contains(startWaveBtn, pos)) {
        return "start_wave";
    }
    if (contains(newMapBtn, pos)) {
        return "new_map";
    }
    if (contains(playFullscreenBtn, pos)) {
        return "toggle_fullscreen";
    }
    return "none";
}

void Hud::drawGameOver(sf::RenderWindow& window, int wave) const {
    const float w = static_cast<float>(window.getSize().x);
    const float h = static_cast<float>(window.getSize().y);

    sf::RectangleShape bg(sf::Vector2f(w, h));
    bg.setFillColor(sf::Color(18, 24, 32));
    window.draw(bg);

    sf::RectangleShape panel(sf::Vector2f(480.f, 360.f));
    panel.setPosition((w - 480.f) * 0.5f, (h - 360.f) * 0.5f);
    panel.setFillColor(sf::Color(28, 38, 52));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(70, 110, 150));
    window.draw(panel);

    const float px = panel.getPosition().x;
    const float py = panel.getPosition().y;
    const auto centerX = [&](const std::string& text, float scale) {
        const float textW = static_cast<float>(text.size()) * 6.f * scale;
        return px + (480.f - textW) * 0.5f;
    };

    drawLabel(window, "GAME OVER", centerX("GAME OVER", 3.f), py + 36.f, 3.f,
              sf::Color(235, 80, 70));
    drawLabel(window, "NO HEARTS LEFT", centerX("NO HEARTS LEFT", 2.f), py + 90.f, 2.f,
              sf::Color(220, 225, 235));
    const std::string waveLine = "WAVE " + std::to_string(wave);
    drawLabel(window, waveLine, centerX(waveLine, 2.5f), py + 130.f, 2.5f,
              sf::Color(230, 120, 230));

    gameOverAgainBtn = makeRect(px + 90.f, py + 190.f, 300.f, 52.f);
    drawButton(window, gameOverAgainBtn, "PLAY AGAIN", sf::Color(40, 130, 90));

    gameOverMenuBtn = makeRect(px + 90.f, py + 260.f, 300.f, 52.f);
    drawButton(window, gameOverMenuBtn, "MAIN MENU", sf::Color(50, 70, 95));
}

std::string Hud::handleGameOverClick(sf::Vector2f pos) const {
    if (contains(gameOverAgainBtn, pos)) {
        return "again";
    }
    if (contains(gameOverMenuBtn, pos)) {
        return "menu";
    }
    return "none";
}

bool Hud::clickedMapDemo(sf::Vector2f pos) const {
    return contains(mapDemoBtn, pos);
}

void Hud::drawMapDemo(sf::RenderWindow& window, const std::string& name,
                      const std::string& rule, bool valid, int index, int count) const {
    const float w = static_cast<float>(window.getSize().x);
    const float h = static_cast<float>(window.getSize().y);
    const float x = w - kSidebarWidth;

    // Same sidebar background as during play.
    sf::RectangleShape side(sf::Vector2f(kSidebarWidth, h));
    side.setPosition(x, 0.f);
    side.setFillColor(sf::Color(22, 28, 38));
    window.draw(side);

    sf::RectangleShape div(sf::Vector2f(2.f, h));
    div.setPosition(x, 0.f);
    div.setFillColor(sf::Color(55, 75, 100));
    window.draw(div);

    float y = 18.f;
    drawLabel(window, "MAP RULES DEMO", x + 18.f, y, 2.f, sf::Color(160, 175, 195));
    y += 26.f;
    drawLabel(window, "EXAMPLE " + std::to_string(index + 1) + "/" + std::to_string(count),
              x + 18.f, y, 1.6f, sf::Color(120, 140, 160));
    y += 34.f;

    drawLabel(window, name, x + 18.f, y, 2.5f, sf::Color::White);
    y += 40.f;

    // The result comes straight from Map::isValid().
    drawLabel(window, "MAP::ISVALID():", x + 18.f, y, 1.6f, sf::Color(160, 175, 195));
    y += 20.f;
    drawLabel(window, valid ? "VALID" : "INVALID", x + 18.f, y, 3.f,
              valid ? sf::Color(80, 210, 110) : sf::Color(235, 80, 70));
    y += 44.f;

    // The rule, one line per '\n'.
    drawLabel(window, "RULE:", x + 18.f, y, 1.6f, sf::Color(160, 175, 195));
    y += 20.f;
    std::size_t start = 0;
    while (start <= rule.size()) {
        const std::size_t end = rule.find('\n', start);
        const std::string line = rule.substr(start, end == std::string::npos ? std::string::npos
                                                                              : end - start);
        drawLabel(window, line, x + 18.f, y, 1.6f, sf::Color(220, 225, 235));
        y += 18.f;
        if (end == std::string::npos) {
            break;
        }
        start = end + 1;
    }
    y += 24.f;

    const float half = (kSidebarWidth - 28.f - 8.f) * 0.5f;
    demoPrevBtn = makeRect(x + 14.f, y, half, 40.f);
    drawButton(window, demoPrevBtn, "PREV", sf::Color(50, 70, 95));
    demoNextBtn = makeRect(x + 14.f + half + 8.f, y, half, 40.f);
    drawButton(window, demoNextBtn, "NEXT", sf::Color(50, 70, 95));
    y += 48.f;
    demoBackBtn = makeRect(x + 14.f, y, kSidebarWidth - 28.f, 40.f);
    drawButton(window, demoBackBtn, "BACK TO MENU", sf::Color(120, 50, 140));
    y += 54.f;
    drawLabel(window, "OR ARROW KEYS / ESC", x + 18.f, y, 1.6f, sf::Color(120, 140, 160));
}

std::string Hud::handleMapDemoClick(sf::Vector2f pos) const {
    if (contains(demoPrevBtn, pos)) {
        return "prev";
    }
    if (contains(demoNextBtn, pos)) {
        return "next";
    }
    if (contains(demoBackBtn, pos)) {
        return "back";
    }
    return "none";
}
