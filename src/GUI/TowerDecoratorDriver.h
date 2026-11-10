/**
 * @file TowerDecoratorDriver.h
 * @brief Assignment 3, Part 3 (Decorator). Driver: decorators changing a tower as it is upgraded.
 */
#ifndef TowerDecoratorDriver_h
#define TowerDecoratorDriver_h

/**
 * @brief Driver: decorators changing a tower as it is upgraded.
 *
 * Shows a tower being wrapped in decorators at run time (level up, splash,
 * burning, freezing) and how each one changes its stats and the effect of its
 * shots on critters.
 */
class TowerDecoratorDriver {
public:
    /** @brief Opens the demo window and runs until it is closed. */
    void run() const;
};

#endif // TowerDecoratorDriver_h
