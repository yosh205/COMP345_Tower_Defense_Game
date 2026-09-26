/**
 * @file main.cpp
 * @brief Game loop: start menu → map + shop HUD → click to place/upgrade towers.
 *
 * Map fills the play area (left of the sidebar). F11 / button toggles fullscreen.
 * Critters are deferred to a later part.
 */
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "GUI/Hud.h"
#include "GUI/MapView.h"
#include "GUI/TowerView.h"
#include "Map/Map.h"
#include "Map/MapGenerator.h"
#include "Tower/AreaDamageTower.h"
#include "Tower/DirectDamageTower.h"
#include "Tower/SlowingTower.h"
#include "Tower/Tower.h"

namespace {

enum class AppState { StartMenu, Playing };

bool isOccupied(const std::vector<std::unique_ptr<Tower>>& towers, int row, int col) {
    for (const auto& t : towers) {
        if (t->getRow() == row && t->getCol() == col) {
            return true;
        }
    }
    return false;
}

int findTowerAt(const std::vector<std::unique_ptr<Tower>>& towers, int row, int col) {
    for (std::size_t i = 0; i < towers.size(); ++i) {
        if (towers[i]->getRow() == row && towers[i]->getCol() == col) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void drawPlacementGhost(sf::RenderWindow& window, int row, int col, float cellSize,
                        float originX, float originY, ShopSelection sel, bool valid) {
    if (sel == ShopSelection::None || row < 0) {
        return;
    }
    sf::CircleShape ghost(cellSize * 0.32f);
    ghost.setOrigin(ghost.getRadius(), ghost.getRadius());
    ghost.setPosition(originX + col * cellSize + cellSize * 0.5f,
                      originY + row * cellSize + cellSize * 0.5f);
    ghost.setFillColor(valid ? sf::Color(255, 255, 255, 70) : sf::Color(220, 50, 50, 90));
    ghost.setOutlineThickness(2.f);
    ghost.setOutlineColor(valid ? sf::Color(255, 255, 255, 180) : sf::Color(255, 80, 80));
    window.draw(ghost);
}

std::unique_ptr<Tower> makeTower(ShopSelection sel, int row, int col) {
    switch (sel) {
        case ShopSelection::Direct: return std::make_unique<DirectDamageTower>(row, col);
        case ShopSelection::Area:   return std::make_unique<AreaDamageTower>(row, col);
        case ShopSelection::Slow:   return std::make_unique<SlowingTower>(row, col);
        default: return nullptr;
    }
}

} // namespace

int main() {
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const sf::VideoMode windowed(1280, 720);

    sf::RenderWindow window(windowed, "Tower Defense", sf::Style::Default);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    Hud hud;
    TowerView towerView;
    AppState state = AppState::StartMenu;

    Map map(4, 4);
    MapView mapView(40.f);
    std::vector<std::unique_ptr<Tower>> towers;
    int selectedTower = -1;
    int ghostRow = -1;
    int ghostCol = -1;
    bool fullscreen = false;

    auto layoutMap = [&]() {
        if (state != AppState::Playing) {
            return;
        }
        const float winW = static_cast<float>(window.getSize().x);
        const float winH = static_cast<float>(window.getSize().y);
        const float mapAreaW = std::max(1.f, winW - Hud::kSidebarWidth);
        const float mapAreaH = std::max(1.f, winH);
        const int rows = map.getLength();
        const int cols = map.getWidth();

        // Fill the play area as much as possible while keeping square cells.
        const float cell = std::min(mapAreaW / static_cast<float>(cols),
                                    mapAreaH / static_cast<float>(rows));
        mapView.setCellSize(std::max(4.f, cell));
        const float usedW = mapView.getCellSize() * static_cast<float>(cols);
        const float usedH = mapView.getCellSize() * static_cast<float>(rows);
        mapView.setOrigin((mapAreaW - usedW) * 0.5f, (mapAreaH - usedH) * 0.5f);
    };

    auto applyDisplayMode = [&]() {
        const sf::Vector2u prev = window.getSize();
        (void)prev;
        if (fullscreen) {
            window.create(desktop, "Tower Defense", sf::Style::Fullscreen);
        } else {
            window.create(windowed, "Tower Defense", sf::Style::Default);
        }
        window.setFramerateLimit(60);
        window.setVerticalSyncEnabled(true);
        hud.setWantsFullscreen(fullscreen);
        layoutMap();
    };

    auto beginPlay = [&]() {
        map = generateRandomMap(hud.getMapRows(), hud.getMapCols());
        towers.clear();
        selectedTower = -1;
        hud.setGold(500);
        hud.clearShopSelection();
        state = AppState::Playing;
        if (hud.wantsFullscreen() != fullscreen) {
            fullscreen = hud.wantsFullscreen();
            applyDisplayMode();
        } else {
            layoutMap();
        }
    };

    auto toggleFullscreen = [&]() {
        fullscreen = !fullscreen;
        applyDisplayMode();
    };

    while (window.isOpen()) {
        const sf::Vector2i mouse = sf::Mouse::getPosition(window);
        const sf::Vector2f mouseF(static_cast<float>(mouse.x), static_cast<float>(mouse.y));
        const float cellSize = mapView.getCellSize();
        const float ox = mapView.getOriginX();
        const float oy = mapView.getOriginY();
        const float mapAreaW =
            static_cast<float>(window.getSize().x) - Hud::kSidebarWidth;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed &&
                       event.key.code == sf::Keyboard::F11) {
                toggleFullscreen();
            } else if (event.type == sf::Event::Resized) {
                // Keep a 1:1 pixel view so UI coordinates match the window.
                sf::FloatRect visible(0.f, 0.f,
                                      static_cast<float>(event.size.width),
                                      static_cast<float>(event.size.height));
                window.setView(sf::View(visible));
                layoutMap();
            }

            if (state == AppState::StartMenu) {
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    // Start-menu fullscreen toggle may change preference only.
                    const bool wasFs = hud.wantsFullscreen();
                    if (hud.handleStartClick(mouseF)) {
                        beginPlay();
                    } else if (hud.wantsFullscreen() != wasFs) {
                        // Preview fullscreen from the menu immediately.
                        fullscreen = hud.wantsFullscreen();
                        applyDisplayMode();
                    }
                }
                continue;
            }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                if (mouseF.x >= mapAreaW) {
                    const Tower* sel =
                        (selectedTower >= 0)
                            ? towers[static_cast<std::size_t>(selectedTower)].get()
                            : nullptr;
                    const std::string action = hud.handlePlayClick(mouseF, sel);

                    if (action == "upgrade" && selectedTower >= 0) {
                        Tower& t = *towers[static_cast<std::size_t>(selectedTower)];
                        const int cost = t.getUpgradeCost();
                        if (t.canUpgrade() && hud.getGold() >= cost) {
                            hud.setGold(hud.getGold() - cost);
                            t.upgrade();
                        }
                    } else if (action == "sell" && selectedTower >= 0) {
                        Tower& t = *towers[static_cast<std::size_t>(selectedTower)];
                        hud.setGold(hud.getGold() + t.getRefundValue());
                        towers.erase(towers.begin() + selectedTower);
                        selectedTower = -1;
                        hud.clearShopSelection();
                    } else if (action == "new_map") {
                        map = generateRandomMap(hud.getMapRows(), hud.getMapCols());
                        towers.clear();
                        selectedTower = -1;
                        hud.clearShopSelection();
                        layoutMap();
                    } else if (action == "toggle_fullscreen") {
                        toggleFullscreen();
                    } else if (action == "buy_direct" || action == "buy_area" ||
                               action == "buy_slow") {
                        selectedTower = -1;
                    }
                } else {
                    const int col = static_cast<int>((mouseF.x - ox) / cellSize);
                    const int row = static_cast<int>((mouseF.y - oy) / cellSize);

                    if (!map.isInBounds(row, col)) {
                        selectedTower = -1;
                    } else if (hud.getShopSelection() != ShopSelection::None) {
                        const int price = Hud::priceOf(hud.getShopSelection());
                        if (map.canPlaceTower(row, col) && !isOccupied(towers, row, col) &&
                            hud.getGold() >= price) {
                            auto tower = makeTower(hud.getShopSelection(), row, col);
                            if (tower) {
                                hud.setGold(hud.getGold() - price);
                                towers.push_back(std::move(tower));
                                selectedTower = static_cast<int>(towers.size()) - 1;
                                hud.clearShopSelection();
                            }
                        }
                    } else {
                        selectedTower = findTowerAt(towers, row, col);
                    }
                }
            } else if (event.type == sf::Event::MouseButtonPressed &&
                       event.mouseButton.button == sf::Mouse::Right) {
                hud.clearShopSelection();
                selectedTower = -1;
            }
        }

        ghostRow = -1;
        ghostCol = -1;
        if (state == AppState::Playing && hud.getShopSelection() != ShopSelection::None &&
            mouseF.x < mapAreaW) {
            ghostCol = static_cast<int>((mouseF.x - ox) / cellSize);
            ghostRow = static_cast<int>((mouseF.y - oy) / cellSize);
        }

        window.clear(sf::Color(12, 16, 22));

        if (state == AppState::StartMenu) {
            hud.drawStartMenu(window);
        } else {
            // Dim the unused letterbox around the map.
            sf::RectangleShape mapBg(sf::Vector2f(mapAreaW, static_cast<float>(window.getSize().y)));
            mapBg.setFillColor(sf::Color(16, 22, 30));
            window.draw(mapBg);

            mapView.draw(window, map);
            towerView.draw(window, towers, cellSize, ox, oy, selectedTower);

            if (ghostRow >= 0 && map.isInBounds(ghostRow, ghostCol)) {
                const bool ok = map.canPlaceTower(ghostRow, ghostCol) &&
                                !isOccupied(towers, ghostRow, ghostCol) &&
                                hud.getGold() >= Hud::priceOf(hud.getShopSelection());
                drawPlacementGhost(window, ghostRow, ghostCol, cellSize, ox, oy,
                                   hud.getShopSelection(), ok);
            }

            const Tower* sel =
                (selectedTower >= 0)
                    ? towers[static_cast<std::size_t>(selectedTower)].get()
                    : nullptr;
            hud.drawPlayHud(window, sel);
        }

        window.display();
    }
    return 0;
}
