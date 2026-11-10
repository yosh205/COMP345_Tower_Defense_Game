/**
 * @file CritterFactoryDriver.h
 * @brief Assignment 3, Part 2 (Factory). Driver: the critter factory creating waves that get harder.
 */
#ifndef CritterFactoryDriver_h
#define CritterFactoryDriver_h

/**
 * @brief Driver: the critter factory creating waves that get harder.
 *
 * Shows that CritterFactory creates the critters of each wave from the wave
 * number: the kinds, how many of each, and their hit points and speed, so a
 * later wave is harder to destroy than an earlier one.
 */
class CritterFactoryDriver {
public:
    /** @brief Opens the demo window and runs until it is closed. */
    void run() const;
};

#endif // CritterFactoryDriver_h
