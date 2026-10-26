/**
 * @file CritterObserverDriver.h
 * @brief Driver: shows CritterObserver redrawing after critters are added, removed or changed.
 */
#ifndef CritterObserverDriver_h
#define CritterObserverDriver_h

/**
 * @brief Creates critters, plugs a CritterObserver into them, then adds and removes them.
 *
 * Opens its own window on a valid map. The first critter is plugged in at the
 * start. Keys add another critter, remove the last one, damage a watched
 * critter, or move the watched critters along the path. Adding and removing
 * redisplay the view; damage and movement do it through notify() and update().
 */
class CritterObserverDriver {
public:
    /** @brief Opens the demo window and runs until it is closed. */
    void run() const;
};

#endif // CritterObserverDriver_h
