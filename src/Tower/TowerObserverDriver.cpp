/**
 * @file TowerObserverDriver.cpp
 * @brief Driver: tower + TowerObserver, then two upgrades that each redraw the view.
 */
#include "TowerObserverDriver.h"

#include "DirectDamageTower.h"
#include "TowerObserver.h"

#include <ostream>

void TowerObserverDriver::run(std::ostream& out) const {
    // The tower is created first so it is still alive when the observer
    // destructor detaches itself.
    DirectDamageTower tower(1, 4);
    TowerObserver observer(out);
    observer.plugInto(tower);

    // Level 1 -> 2. upgrade() notifies the observer, which redisplays the view.
    tower.upgrade();
    // Level 2 -> 3. A second change, so the view is displayed again.
    tower.upgrade();
}
