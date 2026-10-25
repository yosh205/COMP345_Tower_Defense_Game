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

/*This method renders the critter onto the window*/
void CritterObserver::showOn(sf::RenderWindow& targetWindow, const Map& targetMap) {
    window = &targetWindow;
    map = &targetMap;
    render();
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

static const char* kindString(CritterKind k) {
    switch (k) {
        case CritterKind::Armored: return "Armored";
        case CritterKind::Fast: return "Fast";
        default: return "Normal";
    }
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
    for (const Critter* critter : watched) {
        drawCritter(*critter, cellSize, originX, originY);
    }
    window->setTitle("Critter observer - watching " + std::to_string(watched.size()) + " critter(s)");
    window->display();
}

