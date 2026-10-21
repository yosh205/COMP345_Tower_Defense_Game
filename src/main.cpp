/**
 * @file main.cpp
 * @brief Game loop: start menu → map + shop HUD → click to place/upgrade towers.
 *
 * Map fills the play area (left of the sidebar). F11 / button toggles fullscreen.
 * Escaping critters cost half a heart. At 0 hearts the game over screen is shown.
 */
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "GUI/Hud.h"
#include "GUI/MapView.h"
#include "GUI/TowerView.h"
#include "GUI/CritterView.h"
#include "Map/Map.h"
#include "Map/MapExamples.h"
#include "Map/MapGenerator.h"
#include "Tower/AreaDamageTower.h"
#include "Tower/DirectDamageTower.h"
#include "Tower/SlowingTower.h"
#include "Tower/Tower.h"
#include "Tower/TowerObserverDriver.h"
#include "Critter/Critter.h"
#include "Critter/CritterGroupGenerator.h"
namespace {

enum class AppState { StartMenu, MapDemo, Playing, GameOver };

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

/**
 * @brief Prints every example map and its Map::isValid() result to the console.
 * @param examples The maps shown on the Map Rules Demo screen.
 */
void printMapExamples(const std::vector<MapExample>& examples) {
    std::cout << "Map rules demo (Map::isValid()):\n";
    for (const MapExample& example : examples) {
        std::cout << "  " << example.name << ": "
                  << (example.map.isValid() ? "VALID" : "INVALID") << "\n";
    }
}

} // namespace

/**
 * @brief Driver: opens the window and runs the game loop.
 *
 * Shows the start menu, then each frame while a wave runs: spawn the next critter,
 * move critters (paying rewards / applying theft), let towers shoot, handle
 * clicks in the shop and on the map, and draw everything.
 * @return 0 when the window is closed.
 */
int main() {
    // Console demonstration of the tower observer: each upgrade redisplays the view.
    std::cout << "Tower observer driver (the view is redrawn after each change):\n";
    TowerObserverDriver observerDemo;
    observerDemo.run(std::cout);
    std::cout << "\n";

    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const sf::VideoMode windowed(1280, 720);

    sf::RenderWindow window(windowed, "Tower Defense", sf::Style::Default);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    Hud hud;
    TowerView towerView;
    CritterView critterView;
    AppState state = AppState::StartMenu;

    sf::Clock clock;        //Frame timer
    float spawnTimer = 0.f;
    const float kSpawnInterval = 1.0f;

    // The current wave. Nothing spawns until the player clicks Start Wave;
    // waveInProgress is true from then until every critter is killed or escaped.
    CritterGroupGenerator wave(1);
    int waveNumber = 0;          // 0 = no wave played yet on this map
    bool waveInProgress = false;

    // Start the next wave: a new group whose critters are stronger and faster
    // (the generator scales hit points, speed and level with the wave number).
    auto startWave = [&]() {
        if (waveInProgress) {
            return;
        }
        ++waveNumber;
        wave = CritterGroupGenerator(waveNumber);
        spawnTimer = kSpawnInterval;  // first critter enters right away
        waveInProgress = true;
        hud.refillHealth();  // 3 hearts for this wave
    };

    // Forget any wave on the map (used for a new game or a new map).
    auto resetWaves = [&]() {
        wave = CritterGroupGenerator(1);  // replaces the old group; nothing spawns until Start Wave
        waveNumber = 0;
        waveInProgress = false;
        spawnTimer = 0.f;
        hud.refillHealth();
    };

    Map map(4, 4);
    MapView mapView(40.f);

    // Map rules demo (Part 1): hand-built maps showing what Map::isValid()
    // accepts and rejects. Opened from the start menu.
    const std::vector<MapExample> mapExamples = makeMapExamples();
    std::size_t exampleIndex = 0;
    MapView exampleView(40.f);
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

        resetWaves();

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
        const float dt = clock.restart().asSeconds();

        const sf::Vector2i mouse = sf::Mouse::getPosition(window);
        const sf::Vector2f mouseF(static_cast<float>(mouse.x), static_cast<float>(mouse.y));
        const float cellSize = mapView.getCellSize();
        const float ox = mapView.getOriginX();
        const float oy = mapView.getOriginY();
        const float mapAreaW =
            static_cast<float>(window.getSize().x) - Hud::kSidebarWidth;

        // Critters only spawn, move and get shot while a wave is running.
        if (state == AppState::Playing && waveInProgress) {
            spawnTimer += dt;
            if (spawnTimer >= kSpawnInterval) {
                wave.findNextCritter(map.findPath());
                spawnTimer = 0.f;
            }

            wave.moveAlongPath(map.findPath(), dt);

            // --- Coins (Part 3) ---
            // moveAlongPath() just removed critters that were killed (reward) or
            // reached the exit (theft). Apply both to the player's gold.
            // Gold never drops below 0.
            const int earned = wave.collectCoinsEarned();
            const int stolen = wave.collectCoinsStolen();
            if (earned != 0 || stolen != 0) {
                hud.setGold(std::max(0, hud.getGold() + earned - stolen));
            }

            // Each escaped critter removes half a heart. At 0 the run is over.
            const int escaped = wave.collectCrittersEscaped();
            if (escaped > 0) {
                hud.loseHalfHearts(escaped);
            }
            if (hud.isDefeated()) {
                waveInProgress = false;
                state = AppState::GameOver;
            } else {
                // --- Towers attack critters (Part 2) ---
                // Get pointers to the critters on the map. Towers need pointers (not
                // copies) so the damage and slow effects change the real critters.
                // The list is rebuilt every frame because critters are added and
                // removed by findNextCritter() and moveAlongPath() above.
                std::vector<Critter*> targets = wave.getActiveCritterPointers();

                // Tower::update() does the whole attack for one tower:
                //   1. counts down its cooldown (rate of fire),
                //   2. detects the living critters within its range,
                //   3. selects the one furthest along the path,
                //   4. shoots it (direct damage, area damage, or slow, by tower type).
                // Dead critters are skipped here and removed by moveAlongPath() next frame.
                for (auto& tower : towers) {
                    tower->update(dt, targets);
                }

                // --- End of wave ---
                // Every critter has been killed or has escaped: the Start Wave
                // button comes back for the next (harder) wave.
                if (wave.crittersRemaining() == 0) {
                    waveInProgress = false;
                }
            }
        }

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
                    if (hud.clickedMapDemo(mouseF)) {
                        state = AppState::MapDemo;
                        exampleIndex = 0;
                        printMapExamples(mapExamples);
                        continue;
                    }
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

            if (state == AppState::MapDemo) {
                // Sidebar buttons, or arrow keys / Esc, browse the example maps.
                std::string action = "none";
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    action = hud.handleMapDemoClick(mouseF);
                } else if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Left) action = "prev";
                    if (event.key.code == sf::Keyboard::Right) action = "next";
                    if (event.key.code == sf::Keyboard::Escape) action = "back";
                }

                const std::size_t count = mapExamples.size();
                if (action == "next") {
                    exampleIndex = (exampleIndex + 1) % count;
                } else if (action == "prev") {
                    exampleIndex = (exampleIndex + count - 1) % count;
                } else if (action == "back") {
                    state = AppState::StartMenu;
                }
                continue;
            }

            if (state == AppState::GameOver) {
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    const std::string action = hud.handleGameOverClick(mouseF);
                    if (action == "again") {
                        beginPlay();
                    } else if (action == "menu") {
                        towers.clear();
                        selectedTower = -1;
                        hud.clearShopSelection();
                        resetWaves();
                        state = AppState::StartMenu;
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
                        resetWaves();  // old critters would be on the old map's path
                        selectedTower = -1;
                        hud.clearShopSelection();
                        layoutMap();
                    } else if (action == "start_wave") {
                        startWave();
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
        } else if (state == AppState::MapDemo) {
            // Draw the current example map centred in the area left of the
            // sidebar, with its isValid() result and rule in the sidebar.
            const MapExample& example = mapExamples[exampleIndex];
            const float areaH = static_cast<float>(window.getSize().y);
            const float rows = static_cast<float>(example.map.getLength());
            const float cols = static_cast<float>(example.map.getWidth());
            const float cell = 0.8f * std::min(mapAreaW / cols, areaH / rows);
            exampleView.setCellSize(cell);
            exampleView.setOrigin((mapAreaW - cell * cols) * 0.5f,
                                  (areaH - cell * rows) * 0.5f);
            exampleView.draw(window, example.map);
            hud.drawMapDemo(window, example.name, example.rule, example.map.isValid(),
                            static_cast<int>(exampleIndex),
                            static_cast<int>(mapExamples.size()));
        } else if (state == AppState::GameOver) {
            hud.drawGameOver(window, waveNumber);
        } else {
            // Dim the unused letterbox around the map.
            sf::RectangleShape mapBg(sf::Vector2f(mapAreaW, static_cast<float>(window.getSize().y)));
            mapBg.setFillColor(sf::Color(16, 22, 30));
            window.draw(mapBg);

            mapView.draw(window, map);
            towerView.draw(window, towers, cellSize, ox, oy, selectedTower);
            critterView.draw(window, wave.getActiveCritters(), cellSize, ox, oy);

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
            // Show the wave number and either the Start Wave button or critters left.
            hud.setWaveInfo(waveNumber,
                            waveInProgress ? wave.crittersRemaining() : 0,
                            waveInProgress);
            hud.drawPlayHud(window, sel);
        }
        
        window.display();
    }
    return 0;
}
