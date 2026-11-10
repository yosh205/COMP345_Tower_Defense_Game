/**
 * @file TowerStrategyDriver.h
 * @brief Assignment 3, Part 1 (Strategy). Driver: towers with different targeting strategies shooting the same critters.
 */
#ifndef TowerStrategyDriver_h
#define TowerStrategyDriver_h

/**
 * @brief Driver: towers with different targeting strategies shooting the same critters.
 *
 * Shows that the strategy, not the tower class, decides which critter is shot:
 * for example the same critters in range of four towers that each use a different
 * strategy (nearest to the tower, nearest to the exit, strongest, weakest), with
 * the chosen target highlighted, and a key to change a tower's strategy.
 */
class TowerStrategyDriver {
public:
    /** @brief Opens the demo window and runs until it is closed. */
    void run() const;
};

#endif // TowerStrategyDriver_h
