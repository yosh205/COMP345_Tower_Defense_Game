/**
 * @file TowerStrategyDriver.cpp
 * @brief Implements TowerStrategyDriver. Function docs are in TowerStrategyDriver.h.
 */
#include "TowerStrategyDriver.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../Map/MapLibrary.h"
#include "../Tower/DirectDamageTower.h"
#include "../Tower/TargetingStrategy.h"
#include "../Tower/Tower.h"
#include "CritterView.h"
#include "MapView.h"
#include "SimpleText.h"
#include "TowerView.h"

namespace {

/** @brief Builds four stationary demo critters with different HP / path progress. */
std::vector<Critter> makeDemoCritters(const Map& map) {
    const std::vector<Position> path = map.findPath();
    std::vector<Critter> critters;
    if (path.size() < 4) {
        return critters;
    }

    // Strong armored near the entry, weak normal mid-path, fast near the exit,
    // and another normal close to the end — so each strategy picks differently.
    const struct Spec {
        CritterKind kind;
        int hp;
        float speed;
        std::size_t pathIndex;
    } specs[] = {
        {CritterKind::Armored, 80, 0.8f, 1},
        {CritterKind::Normal,  40, 1.0f, path.size() / 3},
        {CritterKind::Fast,    25, 1.6f, (path.size() * 2) / 3},
        {CritterKind::Normal,  55, 1.0f, path.size() - 2},
    };

    for (const Spec& s : specs) {
        const Position& p = path[s.pathIndex];
        Critter c(s.kind, s.hp, s.speed, p.row, p.col, 1);
        c.setPathIndex(static_cast<int>(s.pathIndex));
        c.setProgress(static_cast<float>(s.pathIndex));
        critters.push_back(c);
    }
    return critters;
}

/** @return Short label for the sidebar. */
std::string kindLabel(CritterKind kind) {
    switch (kind) {
        case CritterKind::Normal:  return "NORMAL";
        case CritterKind::Armored: return "ARMORED";
        case CritterKind::Fast:    return "FAST";
    }
    return "?";
}

} // namespace

void TowerStrategyDriver::run() const {
    sf::RenderWindow window(sf::VideoMode(960, 560),
                            "Tower Strategy Demo - Click a tower, S cycles strategy, Esc back");
    window.setFramerateLimit(30);

    Map map = makeMapFromRows({
        "##########",
        "E.......##",
        "#######.##",
        "#######..X",
        "##########",
    });

    // Four Direct towers, each starting on a different strategy.
    std::vector<std::unique_ptr<Tower>> towers;
    towers.push_back(std::make_unique<DirectDamageTower>(1, 2));
    towers.push_back(std::make_unique<DirectDamageTower>(1, 5));
    towers.push_back(std::make_unique<DirectDamageTower>(3, 5));
    towers.push_back(std::make_unique<DirectDamageTower>(3, 7));
    towers[0]->setStrategy(std::make_unique<NearestToExitStrategy>());
    towers[1]->setStrategy(std::make_unique<NearestToTowerStrategy>());
    towers[2]->setStrategy(std::make_unique<StrongestStrategy>());
    towers[3]->setStrategy(std::make_unique<WeakestStrategy>());

    std::vector<Critter> critters = makeDemoCritters(map);
    int selected = 0;

    MapView mapView(48.f);
    mapView.setOrigin(24.f, 72.f);
    TowerView towerView;
    CritterView critterView;

    std::cout << "\nTower strategy demo. Each tower uses a TargetingStrategy to pick a critter.\n"
              << "Click a tower (or Left/Right). S cycles its strategy. Esc back.\n";

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::Resized) {
                window.setView(sf::View(sf::FloatRect(0.f, 0.f,
                                                      static_cast<float>(event.size.width),
                                                      static_cast<float>(event.size.height))));
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                } else if (event.key.code == sf::Keyboard::S && selected >= 0) {
                    towers[static_cast<std::size_t>(selected)]->cycleStrategies();
                    std::cout << "Tower " << selected << " -> "
                              << towers[static_cast<std::size_t>(selected)]->getStrategy() << "\n";
                } else if (event.key.code == sf::Keyboard::Left) {
                    selected = (selected + static_cast<int>(towers.size()) - 1) %
                               static_cast<int>(towers.size());
                } else if (event.key.code == sf::Keyboard::Right) {
                    selected = (selected + 1) % static_cast<int>(towers.size());
                }
            } else if (event.type == sf::Event::MouseButtonPressed &&
                       event.mouseButton.button == sf::Mouse::Left) {
                const float cell = mapView.getCellSize();
                const int col = static_cast<int>((event.mouseButton.x - mapView.getOriginX()) / cell);
                const int row = static_cast<int>((event.mouseButton.y - mapView.getOriginY()) / cell);
                for (std::size_t i = 0; i < towers.size(); ++i) {
                    if (towers[i]->getRow() == row && towers[i]->getCol() == col) {
                        selected = static_cast<int>(i);
                        break;
                    }
                }
            }
        }

        std::vector<Critter*> pointers;
        for (Critter& c : critters) {
            if (c.isAlive()) {
                pointers.push_back(&c);
            }
        }

        window.clear(sf::Color(18, 24, 32));
        SimpleText::draw(window, "TOWER STRATEGY DEMO", 24.f, 18.f, 3.f, sf::Color(220, 235, 255));
        SimpleText::draw(window, "CLICK TOWER    S: CYCLE STRATEGY    LEFT/RIGHT    ESC: BACK",
                         24.f, 48.f, 1.6f, sf::Color(120, 140, 160));

        mapView.draw(window, map);
        towerView.draw(window, towers, mapView.getCellSize(), mapView.getOriginX(),
                       mapView.getOriginY(), selected);
        critterView.draw(window, critters, mapView.getCellSize(), mapView.getOriginX(),
                         mapView.getOriginY());

        // Draw aim lines and a ring on the selected tower's chosen critter.
        if (selected >= 0) {
            const Tower& t = *towers[static_cast<std::size_t>(selected)];
            if (Critter* chosen = t.peekTarget(pointers)) {
                const float cell = mapView.getCellSize();
                const float ox = mapView.getOriginX();
                const float oy = mapView.getOriginY();
                const sf::Vector2f from(ox + t.getCol() * cell + cell * 0.5f,
                                        oy + t.getRow() * cell + cell * 0.5f);
                const sf::Vector2f to(ox + chosen->getCol() * cell + cell * 0.5f,
                                      oy + chosen->getRow() * cell + cell * 0.5f);

                sf::Vertex line[] = {
                    sf::Vertex(from, sf::Color(255, 220, 80, 200)),
                    sf::Vertex(to, sf::Color(255, 220, 80, 200)),
                };
                window.draw(line, 2, sf::Lines);

                sf::CircleShape ring(cell * 0.42f);
                ring.setOrigin(ring.getRadius(), ring.getRadius());
                ring.setPosition(to);
                ring.setFillColor(sf::Color::Transparent);
                ring.setOutlineThickness(3.f);
                ring.setOutlineColor(sf::Color(255, 220, 80));
                window.draw(ring);
            }
        }

        // Sidebar: each tower's strategy and a list of critters.
        const float sx = 540.f;
        SimpleText::draw(window, "TOWERS", sx, 80.f, 2.f, sf::Color(160, 175, 195));
        float y = 110.f;
        for (std::size_t i = 0; i < towers.size(); ++i) {
            const sf::Color color = (static_cast<int>(i) == selected)
                                        ? sf::Color(255, 220, 80)
                                        : sf::Color::White;
            const Critter* aimed = towers[i]->peekTarget(pointers);
            std::string aim = aimed ? (" -> " + kindLabel(aimed->getKind())) : " -> NONE";
            SimpleText::draw(window,
                             "T" + std::to_string(i) + " " + towers[i]->getStrategy() + aim,
                             sx, y, 1.6f, color);
            y += 28.f;
        }

        y += 16.f;
        SimpleText::draw(window, "CRITTERS", sx, y, 2.f, sf::Color(160, 175, 195));
        y += 28.f;
        for (const Critter& c : critters) {
            SimpleText::draw(window,
                             kindLabel(c.getKind()) + " HP " + std::to_string(c.getHitPoints()) +
                                 " PATH " + std::to_string(c.getPathIndex()),
                             sx, y, 1.6f, sf::Color(200, 210, 220));
            y += 24.f;
        }

        window.display();
    }

    std::cout << "Tower strategy demo closed.\n\n";
}
