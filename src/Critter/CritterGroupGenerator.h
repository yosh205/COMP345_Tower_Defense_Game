#ifndef CritterGroupGenerator_h
#define CritterGroupGenerator_h

#include "Critter.h"
#include <string>
#include <vector>
#include <queue>
#include "../Map/Map.h"

class CritterGroupGenerator {
public:
	//Constructor - create a group of critters and put them in a queue
	CritterGroupGenerator(int waveNb); // adds critters to the critterGroup, at the beginning 
	void findNextCritter(const std::vector<Position> path); //returns the first critter in the vector and positions it at entry point
	void moveAlongPath(std::vector<Position> path, float dt); // if a critter has entered the path, it moves along at its given speed
	int deleteCritter(int critterIndex); // removes the critter at critterIndex from the active critters

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

	int activeCrittersLeft();

	/**
	 * @brief Critters of this wave not yet killed or escaped: those waiting to
	 * enter plus those on the map. The wave is over when this reaches 0.
	 */
	int crittersRemaining() const;
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
	int difficultyLevel; // will be increased by 1 each wave, reaching maximum 3
	std::vector<Critter> activeCritters;
	std::queue<Critter> pendingCritters;
	int addCritterToGroup(Critter crit);
	int coinsEarned = 0; // rewards from killed critters, not yet collected
	int coinsStolen = 0; // coins taken by escaped critters, not yet collected
};

#endif