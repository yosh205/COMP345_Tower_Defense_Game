/**
 * @file MapObserverDriver.h
 * @brief Driver: shows MapObserver redrawing the map after each change.
 */
#ifndef MapObserverDriver_h
#define MapObserverDriver_h

/**
 * @brief Creates a map, plugs a MapObserver into it, then changes the map from the keyboard.
 * 
 * Opens its own window. Each key changes the map (place, move or remove a 
 * tower, change a cell, reset the map); the map calls notify() and the 
 * observer's update() redraws the window and logs the update to the console.
 */
class MapObserverDriver {
    public:
    /** @brief Opens the demo window and runs until it is closed */
    void run() const;
};

#endif // MapObserverDriver_h