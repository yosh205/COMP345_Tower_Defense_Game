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
	int deleteCritter(int critterIndex); // this removes the critter from the vector if it is dead (+ rewards player) (replaced by null pointer)
	int rewardCritters(); // rewards critters with player's points if the individual critter's position is the exit spot
	int activeCrittersLeft();
	const std::vector<Critter>& getActiveCritters() const { return activeCritters; }


private:
	int difficultyLevel; // will be increased by 1 each wave, reaching maximum 3
	std::vector<Critter> activeCritters;
	std::queue<Critter> pendingCritters;
	int addCritterToGroup(Critter crit);
	int stolenPoints = 0;
};

#endif