/**
 * @file CritterObserver.cpp
 * @brief Prints and draws the critter view when a watched critter changes.
 */
#include "CritterObserver.h"

#include "../Observer/Observable.h"

#include <algorithm>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>

namespace {

/** @return A short name for kind, used in the text view. */
const char* kindName(CritterKind kind) {
    switch (kind) {
        case CritterKind::Normal:  return "Normal";
        case CritterKind::Armored: return "Armored";
        case CritterKind::Fast:    return "Fast";
    }
    return "Unknown";
}

} // namespace

CritterObserver::CritterObserver(std::ostream& out) : out(out) {}

CritterObserver::~CritterObserver() {
    for (Critter* critter : observedCritters) {
        critter->detach(this);
    }
    observedCritters.clear();
}

void CritterObserver::showOn(sf::RenderWindow& target, const Map& shownMap) {
    window = &target;
    map = &shownMap;
    render();
}

void CritterObserver::plugInto(Critter& critter) {
    if (std::find(observedCritters.begin(), observedCritters.end(), &critter) != observedCritters.end()) {
        return;
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
    printView(critter, "removed");
    render();
}

void CritterObserver::update(Observable* subject) {
    const Critter* critter = dynamic_cast<Critter*>(subject);
    if (critter == nullptr) {
        return;
    }
    if (std::find(observedCritters.begin(), observedCritters.end(), critter) == observedCritters.end()) {
        return;
    }
    printView(*critter, "changed");
    render();
}

void CritterObserver::printView(const Critter& critter, const char* event) {
    std::ostringstream view;
    view << std::fixed << std::setprecision(2);
    view << "----- Critter view (" << event << ") -----\n";
    view << "Kind: " << kindName(critter.getKind()) << "\n";
    view << "Health: " << critter.getHitPoints() << "/" << critter.getMaxHitPoints() << "\n";
    view << "Speed: " << critter.getSpeed() << "\n";
    view << "Position: (" << critter.getRow() << ", " << critter.getCol() << ")\n";
    view << "Watching: " << observedCritters.size() << "\n";
    view << "--------------------------------\n";
    out << view.str();
}

void CritterObserver::render() {
    if (window == nullptr || map == nullptr) {
        return;
    }

    const float winWidth = static_cast<float>(window->getSize().x);
    const float winHeight = static_cast<float>(window->getSize().y);
    const float rows = static_cast<float>(map->getLength());
    const float cols = static_cast<float>(map->getWidth());
    const float cellSize = std::min(winWidth / cols, winHeight / rows);
    const float originX = (winWidth - cols * cellSize) * 0.5f;
    const float originY = (winHeight - rows * cellSize) * 0.5f;
    mapView.setCellSize(cellSize);
    mapView.setOrigin(originX, originY);

    std::vector<Critter> drawn;
    drawn.reserve(observedCritters.size());
    for (const Critter* critter : observedCritters) {
        drawn.push_back(*critter);
    }

    window->clear(sf::Color(12, 16, 22));
    mapView.draw(*window, *map);
    critterView.draw(*window, drawn, cellSize, originX, originY);
    window->setTitle("Critter observer - watching " + std::to_string(observedCritters.size()) +
                     " critter(s)");
    window->display();
}

int CritterObserver::getWatchedCount() const {
    return static_cast<int>(observedCritters.size());
}
