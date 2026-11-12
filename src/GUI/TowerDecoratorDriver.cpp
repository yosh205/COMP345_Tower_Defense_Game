/**
 * @file TowerDecoratorDriver.cpp
 * @brief Implements TowerDecoratorDriver. Function docs are in TowerDecoratorDriver.h.
 */
#include "TowerDecoratorDriver.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../Map/MapLibrary.h"
#include "../Tower/DirectDamageTower.h"
#include "../Tower/TowerDecorator.h"
#include "CritterView.h"
#include "MapView.h"
#include "SimpleText.h"
#include "TowerView.h"

namespace {

std::vector<Critter> makeLine(const Map& map) {
    const std::vector<Position> path = map.findPath();
    std::vector<Critter> critters;
    const CritterKind kinds[] = {CritterKind::Normal, CritterKind::Armored, CritterKind::Fast,
                                 CritterKind::Normal, CritterKind::Armored};
    for (std::size_t i = 0; i < 5 && i + 1 < path.size(); ++i) {
        const Position& p = path[i + 1];
        Critter c(kinds[i], 60, 0.9f, p.row, p.col, 1);
        c.setPathIndex(static_cast<int>(i + 1));
        c.setProgress(static_cast<float>(i + 1));
        critters.push_back(c);
    }
    return critters;
}

void printTower(const Tower& tower) {
    std::cout << "  " << tower.getTypeName() << " LV" << tower.getLevel()
              << " RNG " << tower.getRange() << " POW " << tower.getPower()
              << " effects [" << tower.getEffectTags() << "]\n";
}

} // namespace

void TowerDecoratorDriver::run() const {
    sf::RenderWindow window(sf::VideoMode(900, 520),
                            "Tower Decorator Demo - U upgrade, S splash, B burn, F freeze, Esc back");
    window.setFramerateLimit(30);

    Map map = makeMapFromRows({
        "##########",
        "E.......X#",
        "##########",
    });

    std::unique_ptr<Tower> tower = std::make_unique<DirectDamageTower>(0, 4);
    std::vector<std::unique_ptr<Tower>> towers;
    towers.push_back(std::move(tower));

    std::vector<Critter> critters = makeLine(map);

    MapView mapView(56.f);
    mapView.setOrigin(40.f, 100.f);
    TowerView towerView;
    CritterView critterView;

    std::cout << "\nTower decorator demo. Wrap the tower at run time:\n"
              << "  U LevelUpDecorator   S SplashDecorator\n"
              << "  B BurningDecorator   F FreezingDecorator   Esc back\n";
    printTower(*towers[0]);

    sf::Clock clock;

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
                auto& slot = towers[0];
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                } else if (event.key.code == sf::Keyboard::U && slot->canUpgrade()) {
                    slot = std::make_unique<LevelUpDecorator>(std::move(slot));
                    std::cout << "Wrapped LevelUpDecorator\n";
                    printTower(*slot);
                } else if (event.key.code == sf::Keyboard::S && !slot->hasEffect("SPLASH")) {
                    slot = std::make_unique<SplashDecorator>(std::move(slot));
                    std::cout << "Wrapped SplashDecorator\n";
                    printTower(*slot);
                } else if (event.key.code == sf::Keyboard::B && !slot->hasEffect("BURN")) {
                    slot = std::make_unique<BurningDecorator>(std::move(slot));
                    std::cout << "Wrapped BurningDecorator\n";
                    printTower(*slot);
                } else if (event.key.code == sf::Keyboard::F && !slot->hasEffect("FREEZE")) {
                    slot = std::make_unique<FreezingDecorator>(std::move(slot));
                    std::cout << "Wrapped FreezingDecorator\n";
                    printTower(*slot);
                } else if (event.key.code == sf::Keyboard::R) {
                    towers[0] = std::make_unique<DirectDamageTower>(0, 4);
                    critters = makeLine(map);
                    std::cout << "Reset tower and critters\n";
                    printTower(*towers[0]);
                }
            }
        }

        const float dt = clock.restart().asSeconds();
        std::vector<Critter*> pointers;
        for (Critter& c : critters) {
            if (c.isAlive()) {
                c.updateEffects(dt);
                pointers.push_back(&c);
            }
        }
        towers[0]->update(dt, pointers);

        window.clear(sf::Color(18, 24, 32));
        SimpleText::draw(window, "TOWER DECORATOR DEMO", 24.f, 18.f, 3.f, sf::Color(220, 235, 255));
        SimpleText::draw(window, "U: LEVEL UP   S: SPLASH   B: BURN   F: FREEZE   R: RESET   ESC",
                         24.f, 48.f, 1.6f, sf::Color(120, 140, 160));

        mapView.draw(window, map);
        towerView.draw(window, towers, mapView.getCellSize(), mapView.getOriginX(),
                       mapView.getOriginY(), 0);
        critterView.draw(window, critters, mapView.getCellSize(), mapView.getOriginX(),
                         mapView.getOriginY());

        const Tower& t = *towers[0];
        const float sx = 620.f;
        SimpleText::draw(window, t.getTypeName() + " LV" + std::to_string(t.getLevel()),
                         sx, 100.f, 2.f, sf::Color::White);
        SimpleText::draw(window,
                         "POW " + std::to_string(t.getPower()) + "  RNG " +
                             std::to_string(static_cast<int>(t.getRange() * 10) / 10) + "." +
                             std::to_string(static_cast<int>(t.getRange() * 10) % 10),
                         sx, 132.f, 1.8f, sf::Color(180, 195, 210));
        const std::string tags = t.getEffectTags();
        SimpleText::draw(window, tags.empty() ? "EFFECTS: NONE" : ("EFFECTS: " + tags),
                         sx, 164.f, 1.8f, sf::Color(255, 180, 100));
        SimpleText::draw(window, "CRITTERS LEFT: " + std::to_string(pointers.size()),
                         sx, 204.f, 1.8f, sf::Color(200, 210, 220));

        window.display();
    }

    std::cout << "Tower decorator demo closed.\n\n";
}
