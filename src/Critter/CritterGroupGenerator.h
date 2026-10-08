/**
 * @file CritterGroupGenerator.h
 * @brief Declares CritterGroupGenerator, which creates a wave of critters and runs it on the map.
 */
#ifndef CritterGroupGenerator_h
#define CritterGroupGenerator_h

#include "Critter.h"
#include <string>
#include <vector>
#include <queue>
#include "../Map/Map.h"

/**
 * @brief Creates the group of critters for one wave and moves them along the path.
 *
 * Game rules:
 * - A new generator is created at the start of each wave (when the player clicks
 *   Start Wave). Its critters are adapted to the wave number: their level equals
 *   the wave number, and their hit points and speed grow with it, so every wave
 *   is harder than the one before.
 * - Critters wait in a queue and enter the map one after another at the entry
 *   (findNextCritter()), then walk toward the exit along Map::findPath() (moveAlongPath()).
 * - A critter killed by towers gives the player its reward; a critter that reaches
 *   the exit steals coins. The game collects these with collectCoinsEarned() and
 *   collectCoinsStolen().
 */
class CritterGroupGenerator {
public:
	/**
	 * @brief Creates the critters of a wave and puts them in the waiting queue.
	 *
	 * Each wave has 10 Normal, 5 Armored and 5 Fast critters, in that order.
	 * Hit points grow by 10% of the base value per wave and speed by 0.1 cells/s per wave.
	 * @param waveNb The wave number, starting at 1. Also used as every critter's level.
	 */
	CritterGroupGenerator(int waveNb);

	/**
	 * @brief Moves the next waiting critter onto the map at the entry.
	 * Does nothing when no critters are waiting.
	 * @param path The route from Map::findPath(); the critter is placed on path[0] (the entry).
	 */
	void findNextCritter(const std::vector<Position> path);

	/**
	 * @brief Moves every critter on the map forward along the path for one frame.
	 *
	 * For each critter, in this order:
	 * 1. If it was killed (0 hit points), its reward is added to the coins earned and it is removed.
	 * 2. Otherwise it moves getSpeed() * dt cells toward the exit (slows included), never backwards.
	 * 3. If it reaches the exit, the coins it steals are added to the coins stolen and it is removed.
	 *
	 * @param path The route from Map::findPath(), entry first and exit last.
	 * @param dt   Seconds since the last frame.
	 */
	void moveAlongPath(std::vector<Position> path, float dt);

	/**
	 * @brief Removes the critter at critterIndex from the critters on the map.
	 * Does nothing if the index is out of range. Does not pay any reward or theft.
	 * @param critterIndex Index into getActiveCritters().
	 * @return Always 0.
	 */
	int deleteCritter(int critterIndex);

	/**
	 * @brief Coins earned from critters killed since the last call, then resets to 0.
	 * Game rule: each killed critter gives Critter::getReward() coins (proportional to its level).
	 * Call once per frame and add the result to the player's gold.
	 */
	int collectCoinsEarned();

	/**
	 * @brief Coins stolen by critters that reached the exit since the last call, then resets to 0.
	 * Game rule: each escaped critter steals Critter::getCoinsStolen() coins (based on its strength).
	 * Call once per frame and subtract the result from the player's gold.
	 */
	int collectCoinsStolen();

	/** @return Number of critters currently on the map (not counting those still waiting). */
	int activeCrittersLeft();

	/**
	 * @brief Critters of this wave not yet killed or escaped: those waiting to
	 * enter plus those on the map. The wave is over when this reaches 0.
	 */
	int crittersRemaining() const;

	/** @return Read-only access to the critters on the map, for drawing (see CritterView). */
	const std::vector<Critter>& getActiveCritters() const { return activeCritters; }

	/**
	 * @brief Pointers to the critters currently on the map, for towers to target.
	 *
	 * Towers take Critter* so their shots (damage, slow) change the critters
	 * stored in this group. getActiveCritters() cannot be used for that because
	 * it only gives read-only access.
	 *
	 * The pointers point into activeCritters, a std::vector. When critters are
	 * added or removed (findNextCritter / moveAlongPath) the vector may move its
	 * elements, which would make old pointers invalid. So call this again every
	 * frame instead of keeping the list.
	 *
	 * @return One pointer per active critter, in the same order as getActiveCritters().
	 */
	std::vector<Critter*> getActiveCritterPointers();


private:
	int difficultyLevel;                  ///< Not used yet (the wave number passed to the constructor sets the difficulty).
	std::vector<Critter> activeCritters;  ///< Critters currently walking on the map.
	std::queue<Critter> pendingCritters;  ///< Critters of this wave waiting to enter, in order.

	/**
	 * @brief Adds a critter to the end of the waiting queue.
	 * @return Always 0.
	 */
	int addCritterToGroup(Critter crit);

	int coinsEarned = 0; ///< Rewards from killed critters, not yet collected.
	int coinsStolen = 0; ///< Coins taken by escaped critters, not yet collected.
};

#endif
