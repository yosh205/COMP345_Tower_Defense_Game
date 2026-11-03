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

void Hud::setMapChoices(const std::vector<std::string>& names) {
    mapChoices.assign(1, "RANDOM");
    mapChoices.insert(mapChoices.end(), names.begin(), names.end());
    mapChoice = 0;
}

int Hud::getMapChoice() const { return mapChoice; }

void Hud::setMapChoice(int choice) {
    if (choice >= 0 && choice < static_cast<int>(mapChoices.size())) {
        mapChoice = choice;
    }
}

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

void Hud::drawHeart(sf::RenderWindow& window, float x, float y, float scale, int halves) const {
    // 7x6 bitmap, same block pixels as SimpleText. '#' is part of the heart.
    static const char* rows[6] = {
        ".##.##.",
        "#######",
        "#######",
        ".#####.",
        "..###..",
        "...#...",
    };
    const sf::Color filled(235, 80, 70);
    const sf::Color empty(58, 46, 54);
    sf::RectangleShape pixel(sf::Vector2f(scale, scale));
    for (int row = 0; row < 6; ++row) {
        for (int col = 0; col < 7; ++col) {
            if (rows[row][col] != '#') {
                continue;
            }
            const bool on = halves >= 2 || (halves == 1 && col <= 3);
            pixel.setFillColor(on ? filled : empty);
            pixel.setPosition(x + static_cast<float>(col) * scale,
                              y + static_cast<float>(row) * scale);
            window.draw(pixel);
        }
    }
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

    sf::RectangleShape panel(sf::Vector2f(480.f, 690.f));
    panel.setPosition((w - 480.f) * 0.5f, (h - 690.f) * 0.5f);
    panel.setFillColor(sf::Color(28, 38, 52));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(70, 110, 150));
    window.draw(panel);

    const float px = panel.getPosition().x;
    const float py = panel.getPosition().y;

    drawLabel(window, "TOWER DEFENSE", px + 80.f, py + 28.f, 3.f, sf::Color(220, 235, 255));
    drawLabel(window, "CHOOSE A MAP THEN START", px + 71.f, py + 70.f, 2.f,
              sf::Color(160, 175, 195));

    // Map selector: RANDOM, then the hand-built maps. The arrows wrap around.
    drawLabel(window, "MAP", px + 40.f, py + 125.f, 2.f);
    mapPrevBtn = makeRect(px + 110.f, py + 115.f, 44.f, 40.f);
    mapNextBtn = makeRect(px + 392.f, py + 115.f, 44.f, 40.f);
    drawButton(window, mapPrevBtn, "<", sf::Color(50, 70, 95));
    drawButton(window, mapNextBtn, ">", sf::Color(50, 70, 95));
    const std::string& mapName = mapChoices[static_cast<std::size_t>(mapChoice)];
    const float nameWidth = static_cast<float>(mapName.size()) * 6.f * 2.f;  // 6 font pixels per character
    drawLabel(window, mapName, px + 273.f - nameWidth * 0.5f, py + 128.f, 2.f,
              sf::Color(255, 220, 100));

    // Rows and columns only apply to a random map; a hand-built map has its own size.
    if (mapChoice == 0) {
        drawLabel(window, "ROWS", px + 40.f, py + 185.f, 2.f);
        rowsMinus = makeRect(px + 210.f, py + 175.f, 44.f, 40.f);
        rowsPlus = makeRect(px + 350.f, py + 175.f, 44.f, 40.f);
        drawButton(window, rowsMinus, "-", sf::Color(50, 70, 95));
        drawButton(window, rowsPlus, "+", sf::Color(50, 70, 95));
        drawLabel(window, std::to_string(mapRows), px + 280.f, py + 185.f, 2.5f,
                  sf::Color(255, 220, 100));

        drawLabel(window, "COLUMNS", px + 40.f, py + 245.f, 2.f);
        colsMinus = makeRect(px + 210.f, py + 235.f, 44.f, 40.f);
        colsPlus = makeRect(px + 350.f, py + 235.f, 44.f, 40.f);
        drawButton(window, colsMinus, "-", sf::Color(50, 70, 95));
        drawButton(window, colsPlus, "+", sf::Color(50, 70, 95));
        drawLabel(window, std::to_string(mapCols), px + 280.f, py + 245.f, 2.5f,
                  sf::Color(255, 220, 100));
    } else {
        rowsMinus = sf::FloatRect();
        rowsPlus = sf::FloatRect();
        colsMinus = sf::FloatRect();
        colsPlus = sf::FloatRect();
        drawLabel(window, "THIS MAP HAS A FIXED SIZE", px + 120.f, py + 215.f, 1.6f,
                  sf::Color(120, 140, 160));
    }

    fullscreenBtn = makeRect(px + 80.f, py + 300.f, 320.f, 44.f);
    drawButton(window, fullscreenBtn,
               fullscreenPreferred ? "FULLSCREEN ON" : "FULLSCREEN OFF",
               sf::Color(50, 70, 95), fullscreenPreferred);
    drawLabel(window, "OR PRESS F11 ANYTIME", px + 120.f, py + 352.f, 1.6f,
              sf::Color(120, 140, 160));

    startBtn = makeRect(px + 90.f, py + 385.f, 300.f, 52.f);
    drawButton(window, startBtn, "START GAME", sf::Color(40, 130, 90));

    // Opens the editor: a blank map of the chosen size, or a copy of the chosen map.
    mapEditorBtn = makeRect(px + 90.f, py + 447.f, 300.f, 44.f);
    drawButton(window, mapEditorBtn, "MAP EDITOR", sf::Color(40, 100, 120));

    // Opens the screen that shows which maps Map::isValid() accepts and rejects.
    mapDemoBtn = makeRect(px + 90.f, py + 503.f, 300.f, 44.f);
    drawButton(window, mapDemoBtn, "MAP RULES DEMO", sf::Color(50, 70, 95));

    // Opens the Part 2 driver: a map with a MapObserver that redraws on every change.
    mapObserverDemoBtn = makeRect(px + 90.f, py + 555.f, 300.f, 44.f);
    drawButton(window, mapObserverDemoBtn, "MAP OBSERVER DEMO", sf::Color(50, 70, 95));

    // Opens the Part 3 driver: critters with a CritterObserver that redraws on every change.
    critterObserverDemoBtn = makeRect(px + 90.f, py + 607.f, 300.f, 44.f);
    drawButton(window, critterObserverDemoBtn, "CRITTER OBSERVER DEMO", sf::Color(50, 70, 95));
}

bool Hud::handleStartClick(sf::Vector2f pos) {
    const int choices = static_cast<int>(mapChoices.size());
    if (contains(mapPrevBtn, pos)) {
        mapChoice = (mapChoice + choices - 1) % choices;
        return false;
    }
    if (contains(mapNextBtn, pos)) {
        mapChoice = (mapChoice + 1) % choices;
        return false;
    }
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
    y += 52.f;

    // One short row under gold/wave: label, pixel hearts, then the number.
    // Kept clear of the shop buttons below.
    drawLabel(window, "HEALTH", x + 18.f, y + 2.f, 2.f, sf::Color(160, 175, 195));
    const float heartScale = 3.f;
    const float heartW = 7.f * heartScale;
    const float heartGap = 6.f;
    const float heartsX = x + 18.f + 6.f * 6.f * 2.f + 10.f;
    for (int i = 0; i < kHeartsPerWave; ++i) {
        const int have = healthHalves - i * 2;
        const int fill = have >= 2 ? 2 : (have == 1 ? 1 : 0);
        drawHeart(window, heartsX + i * (heartW + heartGap), y, heartScale, fill);
    }
    const int wholeHearts = healthHalves / 2;
    const std::string healthText = (healthHalves % 2 == 0)
                                        ? std::to_string(wholeHearts)
                                        : (std::to_string(wholeHearts) + ".5");
    drawLabel(window, healthText,
              heartsX + kHeartsPerWave * (heartW + heartGap) + 4.f,
              y + 2.f, 2.f, sf::Color(235, 90, 100));
    y += 32.f;

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
    // A random map is replaced by a new one; a hand-built map starts over.
    drawButton(window, newMapBtn, mapChoice == 0 ? "NEW MAP" : "RESTART MAP",
               sf::Color(50, 70, 95));
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

bool Hud::clickedMapObserverDemo(sf::Vector2f pos) const {
    return contains(mapObserverDemoBtn, pos);
}

bool Hud::clickedCritterObserverDemo(sf::Vector2f pos) const {
    return contains(critterObserverDemoBtn, pos);
}

bool Hud::clickedMapEditor(sf::Vector2f pos) const {
    return contains(mapEditorBtn, pos);
}

void Hud::drawMapEditor(sf::RenderWindow& window, EditorTool tool, bool valid,
                        const std::string& problem, int unused,
                        const std::string& message) const {
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
    drawLabel(window, "MAP EDITOR", x + 18.f, y, 2.5f, sf::Color::White);
    y += 30.f;
    drawLabel(window, "CLICK OR DRAG ON THE MAP", x + 18.f, y, 1.6f, sf::Color(120, 140, 160));
    y += 18.f;
    drawLabel(window, "RIGHT CLICK ERASES", x + 18.f, y, 1.6f, sf::Color(120, 140, 160));
    y += 30.f;

    // Tools, two per row. The selected one has a yellow outline.
    drawLabel(window, "TOOL", x + 18.f, y, 2.f, sf::Color(160, 175, 195));
    y += 22.f;
    const float half = (kSidebarWidth - 28.f - 8.f) * 0.5f;
    editorPathBtn = makeRect(x + 14.f, y, half, 40.f);
    drawButton(window, editorPathBtn, "PATH", sf::Color(150, 125, 80), tool == EditorTool::Path);
    editorEraseBtn = makeRect(x + 14.f + half + 8.f, y, half, 40.f);
    drawButton(window, editorEraseBtn, "ERASE", sf::Color(55, 110, 60), tool == EditorTool::Scenery);
    y += 48.f;
    editorEntryBtn = makeRect(x + 14.f, y, half, 40.f);
    drawButton(window, editorEntryBtn, "ENTRY", sf::Color(50, 90, 170), tool == EditorTool::Entry);
    editorExitBtn = makeRect(x + 14.f + half + 8.f, y, half, 40.f);
    drawButton(window, editorExitBtn, "EXIT", sf::Color(170, 55, 55), tool == EditorTool::Exit);
    y += 60.f;

    // Whether the map can be played, straight from Map::isValid().
    drawLabel(window, valid ? "VALID" : "INVALID", x + 18.f, y, 3.f,
              valid ? sf::Color(80, 210, 110) : sf::Color(235, 80, 70));
    y += 32.f;
    std::string notes = problem;
    if (valid && unused > 0) {
        notes = std::to_string(unused) + " PATH CELL" + (unused == 1 ? "" : "S") +
                " NOT ON\nTHE ROUTE: CRITTERS\nNEVER WALK THERE";
    }
    std::size_t start = 0;
    while (!notes.empty() && start <= notes.size()) {
        const std::size_t end = notes.find('\n', start);
        drawLabel(window, notes.substr(start, end == std::string::npos ? std::string::npos : end - start),
                  x + 18.f, y, 1.6f, sf::Color(220, 225, 235));
        y += 18.f;
        if (end == std::string::npos) {
            break;
        }
        start = end + 1;
    }
    y = std::max(y + 16.f, 300.f);

    editorPlayBtn = makeRect(x + 14.f, y, kSidebarWidth - 28.f, 44.f);
    drawButton(window, editorPlayBtn, "PLAY THIS MAP",
               valid ? sf::Color(40, 130, 90) : sf::Color(60, 65, 75));
    y += 52.f;
    editorSaveBtn = makeRect(x + 14.f, y, half, 40.f);
    drawButton(window, editorSaveBtn, "SAVE", valid ? sf::Color(50, 70, 95) : sf::Color(60, 65, 75));
    editorClearBtn = makeRect(x + 14.f + half + 8.f, y, half, 40.f);
    drawButton(window, editorClearBtn, "CLEAR", sf::Color(140, 55, 55));
    y += 48.f;
    editorBackBtn = makeRect(x + 14.f, y, kSidebarWidth - 28.f, 40.f);
    drawButton(window, editorBackBtn, "BACK TO MENU", sf::Color(120, 50, 140));
    y += 54.f;

    if (!message.empty()) {
        drawLabel(window, message, x + 18.f, y, 1.6f, sf::Color(255, 220, 100));
    }
}

std::string Hud::handleMapEditorClick(sf::Vector2f pos) const {
    if (contains(editorPathBtn, pos)) return "tool_path";
    if (contains(editorEntryBtn, pos)) return "tool_entry";
    if (contains(editorExitBtn, pos)) return "tool_exit";
    if (contains(editorEraseBtn, pos)) return "tool_erase";
    if (contains(editorClearBtn, pos)) return "clear";
    if (contains(editorSaveBtn, pos)) return "save";
    if (contains(editorPlayBtn, pos)) return "play";
    if (contains(editorBackBtn, pos)) return "back";
    return "none";
}
