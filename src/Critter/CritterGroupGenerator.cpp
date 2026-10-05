#include <string>
#include <vector>
#include <queue>
#include "CritterGroupGenerator.h"
#include "../Critter/Critter.h"
#include "../Map/Map.h"
#include <iostream>

CritterGroupGenerator::CritterGroupGenerator(int waveNb) { // for now, hardcoded to always generate 20 critters. Can be refined to have a percentage-based implementation
	//Normal Critters
	for (int i = 0; i < 10; i++) { 
		Critter newCritter(CritterKind::Normal, static_cast<int>(100.f*(1.f+(waveNb/10.0))), 0.4+(0.1*waveNb), 0, 0);
		addCritterToGroup(newCritter);
	}
	//Armored Critters
	for (int i = 0; i < 5; i++) {
		Critter newCritter(CritterKind::Armored, static_cast<int>(130.f * (1.f + (waveNb / 10.0))), 0.2 + (0.1 * waveNb), 0, 0);
		addCritterToGroup(newCritter);
	}
	//Fast Critters
	for (int i = 0; i < 5; i++) {
		Critter newCritter(CritterKind::Fast, static_cast<int>(80.f * (1.f + (waveNb / 10.0))), 0.8 + (0.1 * waveNb), 0, 0);
		addCritterToGroup(newCritter);
	}
	//create a group of critters, how do we determine the amount/kind of critters?
}

int CritterGroupGenerator::addCritterToGroup(Critter crit) {
	CritterGroupGenerator::pendingCritters.push(crit);
	return 0;
}

void CritterGroupGenerator::moveAlongPath(std::vector<Position> path, float dt) {
	if (activeCritters.empty()) {
		return
	}
	else {
		int critterIndexCounter = 0; // keep track of the index of the critter

		for (Critter& critter : activeCritters) {
			critter.updateEffects(dt);
			critter.addProgress(critter.getSpeed()*dt);
			int index = std::min(static_cast<int>(critter.getProgress()), static_cast<int>path.size() - 1);
			critter.setPathIndex(index);
			critter.setPosition(path[index].row, path[index].col);

			if (critter.getRow() == path.at(path.size() - 1).row && critter.getCol() == path.at(path.size() - 1).col) {
				this->rewardCritters();
				//you should then remove that specific critter from the active critter vector
			}
			if (!(critter.isAlive())) {
				this->deleteCritter(critterIndexCounter);
			}
		}
	}
}

void CritterGroupGenerator::findNextCritter(std::vector<Position> path) {

	if (pendingCritters.empty()) {
		return;
	}
	else {
		Critter nextCritter = pendingCritters.front();
		pendingCritters.pop();
		nextCritter.setPathIndex(0);
		nextCritter.setPosition(path[0].row, path[0].col);
		activeCritters.push_back(nextCritter);
	}
}

int CritterGroupGenerator::rewardCritters() { //placeholder, need to add the stealing from player + proportional to strenght aspect
	stolenPoints += 10;
	return stolenPoints;
	//you should then remove that specific critter from the active critter vector
}

int CritterGroupGenerator::deleteCritter(int critterIndex) {
	activeCritters.erase(critterIndex);
	//remove critter at critterIndex and reward the player according to this critter's strength
	return 0;
}


int CritterGroupGenerator::activeCrittersLeft() {
	return activeCritters.empty();
}
