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
	if (activeCritters.empty() || path.size() < 2) {
		return;
	}

	for (int i = static_cast<int>(activeCritters.size()) - 1; i >= 0; --i) {
		Critter& critter = activeCritters[i];
		critter.updateEffects(dt);
		critter.addProgress(critter.getSpeed() * dt);

		float progress = critter.getProgress();
		int currentIndex = static_cast<int>(progress);

		// Check if critter reached or passed the end of the path
		if (currentIndex >= static_cast<int>(path.size()) - 1) {
			// Position at final tile exit
			critter.setPosition(static_cast<float>(path.back().row),
				static_cast<float>(path.back().col));
			this->rewardCritters();
			deleteCritter(i);
			continue;
		}

		// --- SMOOTH MOVEMENT INTERPOLATION (LERP) ---
		float t = progress - static_cast<float>(currentIndex); // Fractional distance (0.0 to 1.0)

		Position currentTile = path[currentIndex];
		Position nextTile = path[currentIndex + 1];

		// Linearly interpolate row and col between current and next tile
		float interpolatedRow = currentTile.row + t * (nextTile.row - currentTile.row);
		float interpolatedCol = currentTile.col + t * (nextTile.col - currentTile.col);

		critter.setPathIndex(currentIndex);
		critter.setPosition(interpolatedRow, interpolatedCol); // Store as float positions!

		// Delete if critter died
		if (!critter.isAlive()) {
			deleteCritter(i);
		}
	}
}

void CritterGroupGenerator::findNextCritter(const std::vector<Position> path) {

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
	if (critterIndex >= 0 && critterIndex < static_cast<int>(activeCritters.size())) {
		activeCritters.erase(activeCritters.begin() + critterIndex);
	}
	//remove critter at critterIndex and reward the player according to this critter's strength
	return 0;	
}


int CritterGroupGenerator::activeCrittersLeft() {
	return static_cast<int>(activeCritters.size());
}
