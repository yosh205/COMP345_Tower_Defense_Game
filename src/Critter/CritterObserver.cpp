/**
 * @file CritterObserver.cpp
 * @brief Prints the critter view when a critter notifies this observer.
 */
#include "CritterObserver.h"

#include "Critter.h"

#include "../Observer/Observable.h"

#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <algorithm>
#include "../GUI/CritterView.h"

CritterObserver::CritterObserver(std::ostream& out) : out(out) {}

/*/CritterObserver::~CritterObserver() {
    if(subject != nullptr) {
        subject->detach(this);
        subject = nullptr;
    }
}*/

/*Destructor*/
CritterObserver::~CritterObserver() {
    for (Critter* critter : watched) {
        critter->detach(this);
    }

}

/*
void CritterObserver::plugInto(Observable& subjectToWatch) {
    if (subject != nullptr && subject != &subjectToWatch) {
        subject->detach(this);
    }
    subject = &subjectToWatch;
    subject->attach(this);
}
*/
void CritterObserver::plugInto(Critter& critter) {
    if (std::find(observedCritters.begin(), observedCritters.end(), &critter) != observedCritters.end()) {
        return; // already watched
    }
    observedCritters.push_back(&critter);
    critter.attach(this);
    printView(critter, "added");
    render();
}

void CritterObserver::unplug(Critter& critter) {
    const auto it = std::find(observedCritters.begin(), observedCritters.end(), &critter);
    if (it == observedCritters.end()) {
        return;
    }
    critter.detach(this);
    observedCritters.erase(it);
    out << "----- Critter removed (" << kindName(critter.getKind()) << ") -----\n"
        << "-----------------------------------\n";
    render();

}

/*Updates the critter viewer after being notified by an Observable Critter*/
void CritterObserver::update(Observable* subject) {
    const Critter* critter = dynamic_cast<Critter*>(subject);
    if (critter == nullptr) {
        return;
    }
    else if (std::find(observedCritters.begin(), observedCritters.end(), critter) == observedCritters.end()) {
        return;
    }
    else {
        printView(*critter, "changed");
        render();

    }
}

/*This method allows us to see a Critter's modifications in text version.*/
void CritterObserver::printView(const Critter& critter, const char* event) {
    std::ostringstream view;
    view << std::fixed << std::setprecision(2);
    view << "----- Critter view ( "<< event << ")----\n";
    view << "Kind: " << critter.getKind() << "\n";
    view << "Health: " << critter.getHitPoints() << "\n";
    view << "Speed: " << critter.getSpeed() << "\n";
    view << "Position: (" << critter.getRow() << ", " << critter.getCol() << ")\n";
    view << "----------------------\n";
    out << view.str();
}


/*This method handles the graphical rendering of the CritterObservers. It is based on CritterView of A1.
*/
void CritterObserver::drawCritter(Critter& critter, float cellSize, float originX, float originY) {

    if (!critter.isAlive()) {
        return;
    }

    const sf::Vector2f c = cellCenter(critter, cellSize, originX, originY);
    const float s = cellSize;

    // 1. Render Critter Base Silhouette by Kind
    const CritterKind kind = critter.getKind();

    if (kind == CritterKind::Normal) {
        // Round Body
        sf::CircleShape body(s * 0.26f);
        body.setOrigin(body.getRadius(), body.getRadius());
        body.setPosition(c);
        body.setFillColor(getColorForKind(kind));
        body.setOutlineThickness(1.5f);
        body.setOutlineColor(sf::Color(140, 30, 30));
        window.draw(body);

        // Inner Core Dot
        sf::CircleShape core(s * 0.09f);
        core.setOrigin(core.getRadius(), core.getRadius());
        core.setPosition(c);
        core.setFillColor(sf::Color(255, 180, 180));
        window.draw(core);

    }
    else if (kind == CritterKind::Armored) {
        // Heavy Octagon / Square Shield Base
        sf::RectangleShape shield(sf::Vector2f(s * 0.46f, s * 0.46f));
        shield.setOrigin(shield.getSize().x * 0.5f, shield.getSize().y * 0.5f);
        shield.setPosition(c);
        shield.setFillColor(getColorForKind(kind));
        shield.setOutlineThickness(2.f);
        shield.setOutlineColor(sf::Color(200, 220, 255));
        window.draw(shield);

        // Inner Armor Plate
        sf::RectangleShape plate(sf::Vector2f(s * 0.24f, s * 0.24f));
        plate.setOrigin(plate.getSize().x * 0.5f, plate.getSize().y * 0.5f);
        plate.setPosition(c);
        plate.setFillColor(sf::Color(30, 50, 90));
        window.draw(plate);

    }
    else { // Fast
        // Forward-facing Triangle / Diamond Silhouette
        sf::ConvexShape diamond;
        diamond.setPointCount(4);
        diamond.setPoint(0, { 0.f, -s * 0.28f });
        diamond.setPoint(1, { s * 0.20f, 0.f });
        diamond.setPoint(2, { 0.f, s * 0.28f });
        diamond.setPoint(3, { -s * 0.20f, 0.f });
        diamond.setPosition(c);
        diamond.setFillColor(getColorForKind(kind));
        diamond.setOutlineThickness(1.5f);
        diamond.setOutlineColor(sf::Color(255, 240, 150));
        window.draw(diamond);

        // Speed Streak Core
        sf::ConvexShape streak;
        streak.setPointCount(4);
        streak.setPoint(0, { 0.f, -s * 0.14f });
        streak.setPoint(1, { s * 0.08f, 0.f });
        streak.setPoint(2, { 0.f, s * 0.14f });
        streak.setPoint(3, { -s * 0.08f, 0.f });
        streak.setPosition(c);
        streak.setFillColor(sf::Color(255, 255, 220));
        window.draw(streak);
    }

    // 2. Health Bar Background
    const float barW = s * 0.65f;
    const float barH = s * 0.08f;
    const float barX = c.x - barW * 0.5f;
    const float barY = c.y - s * 0.36f;

    sf::RectangleShape hpBg(sf::Vector2f(barW, barH));
    hpBg.setPosition(barX, barY);
    hpBg.setFillColor(sf::Color(40, 15, 15));
    hpBg.setOutlineThickness(1.f);
    hpBg.setOutlineColor(sf::Color(15, 15, 20));
    window.draw(hpBg);

    // 3. Health Bar Fill
    float ratio = static_cast<float>(critter.getHitPoints()) /
        static_cast<float>(critter.getMaxHitPoints());
    ratio = std::clamp(ratio, 0.f, 1.f);

    sf::RectangleShape hpFill(sf::Vector2f(barW * ratio, barH));
    hpFill.setPosition(barX, barY);
    hpFill.setFillColor((ratio > 0.4f) ? sf::Color(60, 220, 80) : sf::Color(230, 60, 50));
    window.draw(hpFill);
}


void CritterObserver::render() {
    if (window == nullptr || map == nullptr) {
        return;
    }
    // Fit the map in the window with square cells, centered (same layout as MapObserver).
    const float winWidth = static_cast<float>(window->getSize().x);
    const float winHeight = static_cast<float>(window->getSize().y);
    const float rows = static_cast<float>(map->getLength());
    const float cols = static_cast<float>(map->getWidth());
    const float cellSize = std::min(winWidth / cols, winHeight / rows);
    const float originX = (winWidth - cols * cellSize) * 0.5f;
    const float originY = (winHeight - rows * cellSize) * 0.5f;
    mapView.setCellSize(cellSize);
    mapView.setOrigin(originX, originY);

    window->clear(sf::Color(12, 16, 22));
    mapView.draw(*window, *map);
    for (const Critter* critter : observedCritters) {
        drawCritter(*critter, cellSize, originX, originY);
    }
    window->setTitle("Critter observer - watching " + std::to_string(watched.size()) + " critter(s)");
    window->display();
}



