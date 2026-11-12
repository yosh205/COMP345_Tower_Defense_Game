/**
 * @file CritterFactory.h
 * @brief Assignment 3, Part 2 (Factory pattern): creates the critters of each wave.
 */
#ifndef CritterFactory_h
#define CritterFactory_h

#include <vector>

#include "Critter.h"

/** @brief How many critters of each kind a wave has. */
struct WavePlan {
    int normal = 0;   ///< Normal critters.
    int fast = 0;     ///< Fast critters.
    int armored = 0;  ///< Armored critters.

    /** @return Number of critters in the wave. */
    int total() const { return normal + fast + armored; }
};

/**
 * @brief Factory that creates critters and whole waves from the wave number.
 *
 * The rest of the game never calls the Critter constructor with numbers: it asks
 * the factory for a critter of a kind, or for wave n. Wave n comes after n - 1
 * waves have been destroyed, and every value grows with n, so the further the
 * player gets, the harder the next wave is:
 * - more critters, and tougher kinds join: wave 1 is only Normal critters,
 *   Fast critters join from wave 2 and Armored critters from wave 3;
 * - every critter has more hit points (+10% per wave) and speed (+0.1 cells/s per wave);
 * - its level is the wave number, so its reward and the coins it steals grow too.
 */
class CritterFactory {
public:
    /**
     * @brief Creates one critter for a wave.
     * @param kind   Normal, Fast or Armored.
     * @param waveNb The wave number, starting at 1. Values below 1 count as 1.
     * @return The critter, at cell (0, 0); the wave places it on the entry when it enters.
     */
    static Critter createCritter(CritterKind kind, int waveNb);

    /**
     * @brief Decides how many critters of each kind wave waveNb has.
     * @param waveNb The wave number, starting at 1. Values below 1 count as 1.
     * @return The number of Normal, Fast and Armored critters.
     */
    static WavePlan planWave(int waveNb);

    /**
     * @brief Creates every critter of a wave, in the order they enter the map.
     *
     * Fast critters enter first, then Normal, then Armored, so faster critters
     * are not stuck behind slower ones (one critter per path cell).
     * @param waveNb The wave number, starting at 1. Values below 1 count as 1.
     * @return The critters of the wave, planWave(waveNb).total() of them.
     */
    static std::vector<Critter> createWave(int waveNb);
};

#endif // CritterFactory_h
