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

CritterObserver::CritterObserver(std::ostream& out) : out(out) {}

CritterObserver::~CritterObserver(){
    if(subject != nullptr) {
        subject->detach(this);
        subject = nullptr;
    }
}

void CritterObserver::plugInto(Observable& subjectToWatch) {
    if (subject != nullptr && subject != &subjectToWatch) {
        subject->detach(this);
    }
    subject = &subjectToWatch;
    subject->attach(this);
}

void CritterObserver::update(Observable* changed) {
    const Critter* critter = dynamic_cast<Critter*>(changed);
    if(critter == nullptr){
        return;
    }
    //Remember the object being watched for the destructor.
    subject = changed;

    std::ostringstream view;
    view << std::fixed << std::setprecision(2);
    view << "----- Critter view ----\n";
    view << "Kind: " << critter->getKind() << "\n";
    view << "Health: " << critter->getHealth() << "\n";
    view << "Speed: " << critter->getSpeed() << "\n";
    view << "Damage: " << critter->getDamage() << "\n";
    view << "Position: (" << critter->getX() << ", " << critter->getY() << ")\n";
    view << "----------------------\n";
    out << view.str();
}