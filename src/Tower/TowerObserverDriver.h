/**
 * @file TowerObserverDriver.h
 * @brief Driver that shows the tower observer redrawing after each change.
 */
#ifndef TowerObserverDriver_h
#define TowerObserverDriver_h

#include <iosfwd>

/**
 * @brief Creates a tower, plugs a TowerObserver into it, then changes it.
 *
 * Each upgrade changes the tower's level, range, power and rate of fire.
 * The tower notifies the observer, and the observer prints the tower view
 * again. Nothing is drawn on screen: the view goes to the given stream.
 */
class TowerObserverDriver {
public:
    /**
     * @brief Runs the demonstration.
     * @param out Stream that receives each redraw of the tower view.
     */
    void run(std::ostream& out) const;
};

#endif // TowerObserverDriver_h
