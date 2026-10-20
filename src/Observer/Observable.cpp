/**
 * @file Observable.cpp
 * @brief Implements Observable. 
 */
#include "Observable.h"

#include <algorithm>

Observable::Observable(const Observable& /*other*/) {
    // intentionally empty: observers watch one specific object, so a copy
    // starts with none
}

Observable& Observable::operator=(const Observable& /*other*/) {
    //
    return *this;
}

void Observable::attach(Observer* observer) {
    if (observer == nullptr) {
        return;
    }
    // Ignore duplicates
    if (std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer);
    }
}

void Observable::detach(Observer* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

int Observable::getObserverCount() const {
    return static_cast<int>(observers.size());
}

void Observable::notify() {
    const std::vector<Observer*> current = observers;
    for (Observer* observer : current) {
        observer->update(this);
    }
}