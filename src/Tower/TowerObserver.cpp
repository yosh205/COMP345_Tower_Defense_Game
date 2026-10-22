/**
 * @file TowerObserver.cpp
 * @brief Prints the tower view when a tower notifies this observer.
 */
#include "TowerObserver.h"

#include "Tower.h"

#include "../Observer/Observable.h"

#include <iomanip>
#include <ostream>
#include <sstream>

TowerObserver::TowerObserver(std::ostream& out) : out(out) {}

TowerObserver::~TowerObserver() {
    if (subject != nullptr) {
        subject->detach(this);
        subject = nullptr;
    }
}

void TowerObserver::plugInto(Observable& subjectToWatch) {
    if (subject != nullptr && subject != &subjectToWatch) {
        subject->detach(this);
    }
    subject = &subjectToWatch;
    subject->attach(this);
}

void TowerObserver::update(Observable* changed) {
    const Tower* tower = dynamic_cast<Tower*>(changed);
    if (tower == nullptr) {
        return;
    }
    // Remember who we are watching so the destructor can detach.
    subject = changed;

    std::ostringstream view;
    view << std::fixed << std::setprecision(2);
    view << "----- Tower view -----\n";
    view << "Type: " << tower->getTypeName() << "\n";
    view << "Cell: (" << tower->getRow() << ", " << tower->getCol() << ")\n";
    view << "Level: " << tower->getLevel() << "/" << tower->getMaxLevel() << "\n";
    view << "Range: " << tower->getRange() << "\n";
    view << "Power: " << tower->getPower() << "\n";
    view << "Fire rate: " << tower->getFireRate() << "\n";
    view << "Refund: " << tower->getRefundValue() << "\n";
    view << "----------------------\n";
    out << view.str();
}
