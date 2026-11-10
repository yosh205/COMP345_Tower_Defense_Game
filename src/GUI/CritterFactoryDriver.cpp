/**
 * @file CritterFactoryDriver.cpp
 * @brief Implements CritterFactoryDriver. Function docs are in CritterFactoryDriver.h.
 */
#include "CritterFactoryDriver.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../Critter/CritterFactory.h"
#include "CritterView.h"
#include "SimpleText.h"

namespace {

/** @return value with one decimal, e.g. "1.2". */
std::string oneDecimal(float value) {
    const int tenths = static_cast<int>(std::lround(value * 10.f));
    return std::to_string(tenths / 10) + "." + std::to_string(tenths % 10);
}

/** @return The kind's name in capitals, for the table. */
std::string kindName(CritterKind kind) {
    switch (kind) {
        case CritterKind::Normal:  return "NORMAL";
        case CritterKind::Fast:    return "FAST";
        case CritterKind::Armored: return "ARMORED";
    }
    return "?";
}

/** @brief Prints the wave the factory made, so the console shows it too. */
void printWave(int waveNb) {
    const WavePlan plan = CritterFactory::planWave(waveNb);
    int totalHp = 0;
    for (const Critter& c : CritterFactory::createWave(waveNb)) {
        totalHp += c.getMaxHitPoints();
    }
    std::cout << "Wave " << waveNb << ": " << plan.normal << " Normal, " << plan.fast
              << " Fast, " << plan.armored << " Armored (" << plan.total()
              << " critters, " << totalHp << " hit points in total)\n";
}

} // namespace

void CritterFactoryDriver::run() const {
    sf::RenderWindow window(sf::VideoMode(760, 520),
                            "Critter Factory Demo - Left/Right change the wave, Esc back");
    window.setFramerateLimit(30);

    std::cout << "\nCritter factory demo. CritterFactory creates every wave from its number.\n"
              << "Keys: Left / Right change the wave, Esc back\n";
    int waveNb = 1;
    printWave(waveNb);

    const CritterView critterView;
    const sf::Color label(160, 175, 195);

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
                } else if (event.key.code == sf::Keyboard::Right && waveNb < 99) {
                    printWave(++waveNb);
                } else if (event.key.code == sf::Keyboard::Left && waveNb > 1) {
                    printWave(--waveNb);
                }
            }
        }

        // Everything shown comes from the factory for the current wave number.
        const WavePlan plan = CritterFactory::planWave(waveNb);
        const std::vector<Critter> wave = CritterFactory::createWave(waveNb);

        window.clear(sf::Color(18, 24, 32));
        SimpleText::draw(window, "CRITTER FACTORY: WAVE " + std::to_string(waveNb),
                         24.f, 20.f, 3.f, sf::Color(220, 235, 255));
        SimpleText::draw(window, "WAVES DESTROYED SO FAR: " + std::to_string(waveNb - 1) +
                                     "    LEFT / RIGHT: WAVE    ESC: BACK",
                         24.f, 56.f, 1.6f, sf::Color(120, 140, 160));

        // One row per kind: an icon, how many the wave has, and one critter's stats.
        const float columns[] = {84.f, 214.f, 314.f, 404.f, 514.f, 634.f};
        const char* headers[] = {"KIND", "COUNT", "HP", "SPEED", "REWARD", "STEALS"};
        for (int i = 0; i < 6; ++i) {
            SimpleText::draw(window, headers[i], columns[i], 96.f, 2.f, label);
        }
        const CritterKind kinds[] = {CritterKind::Fast, CritterKind::Normal, CritterKind::Armored};
        const int counts[] = {plan.fast, plan.normal, plan.armored};
        float y = 130.f;
        for (int i = 0; i < 3; ++i) {
            const Critter sample = CritterFactory::createCritter(kinds[i], waveNb);
            const sf::Color color = counts[i] > 0 ? sf::Color::White : sf::Color(90, 100, 115);
            critterView.draw(window, {sample}, 40.f, 24.f, y - 12.f);
            const std::string cells[] = {
                kindName(kinds[i]), std::to_string(counts[i]),
                std::to_string(sample.getMaxHitPoints()), oneDecimal(sample.getBaseSpeed()),
                "$" + std::to_string(sample.getReward()), "$" + std::to_string(sample.getCoinsStolen())};
            for (int c = 0; c < 6; ++c) {
                SimpleText::draw(window, cells[c], columns[c], y, 2.f, color);
            }
            y += 48.f;
        }

        int totalHp = 0;
        for (const Critter& c : wave) {
            totalHp += c.getMaxHitPoints();
        }
        SimpleText::draw(window, "CRITTERS: " + std::to_string(plan.total()) +
                                     "    TOTAL HP: " + std::to_string(totalHp),
                         24.f, 290.f, 2.f, sf::Color(255, 220, 100));

        // The whole wave, in the order the critters enter the map.
        SimpleText::draw(window, "ENTRY ORDER", 24.f, 330.f, 2.f, label);
        const int perLine = 26;
        std::vector<Critter> line = wave;
        for (std::size_t i = 0; i < line.size(); ++i) {
            line[i].setPosition(static_cast<float>(static_cast<int>(i) / perLine),
                                static_cast<float>(static_cast<int>(i) % perLine));
        }
        critterView.draw(window, line, 26.f, 24.f, 356.f);

        window.display();
    }
    std::cout << "Critter factory demo closed.\n\n";
}
