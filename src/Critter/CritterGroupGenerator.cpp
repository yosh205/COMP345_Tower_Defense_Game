#include <string>
#include <vector>
#include <queue>
#include "CritterGroupGenerator.h"
#include "../Critter/Critter.h"
#include "../Map/Map.h"
#include <iostream>

CritterGroupGenerator::CritterGroupGenerator(int waveNb) { // for now, hardcoded to always generate 20 critters. Can be refined to have a percentage-based implementation
	// Every critter's level is the wave number, so its reward and strength
	// (calculated by Critter from the level) grow with each wave.
	//Normal Critters
	for (int i = 0; i < 10; i++) {
		Critter newCritter(CritterKind::Normal, static_cast<int>(100.f*(1.f+(waveNb/10.0))), 0.4+(0.1*waveNb), 0, 0, waveNb);
		addCritterToGroup(newCritter);
	}
	//Armored Critters
	for (int i = 0; i < 5; i++) {
		Critter newCritter(CritterKind::Armored, static_cast<int>(130.f * (1.f + (waveNb / 10.0))), 0.2 + (0.1 * waveNb), 0, 0, waveNb);
		addCritterToGroup(newCritter);
	}
	//Fast Critters
	for (int i = 0; i < 5; i++) {
		Critter newCritter(CritterKind::Fast, static_cast<int>(80.f * (1.f + (waveNb / 10.0))), 0.8 + (0.1 * waveNb), 0, 0, waveNb);
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

		// A critter killed by a tower (last frame) pays its reward and is removed
		// BEFORE it moves, so a dead critter can never walk onto the exit and steal.
		if (!critter.isAlive()) {
			coinsEarned += critter.getReward();
			deleteCritter(i);
			continue;
		}

		critter.updateEffects(dt);
		critter.addProgress(critter.getSpeed() * dt);

		float progress = critter.getProgress();
		int currentIndex = static_cast<int>(progress);

		// Check if critter reached or passed the end of the path
		if (currentIndex >= static_cast<int>(path.size()) - 1) {
			// Position at final tile exit
			critter.setPosition(static_cast<float>(path.back().row),
				static_cast<float>(path.back().col));
			// It got through: it steals coins based on its strength.
			coinsStolen += critter.getCoinsStolen();
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

int CritterGroupGenerator::collectCoinsEarned() {
	// Hand over the total since the last call, then start counting again from 0.
	const int earned = coinsEarned;
	coinsEarned = 0;
	return earned;
}

int CritterGroupGenerator::collectCoinsStolen() {
	const int stolen = coinsStolen;
	coinsStolen = 0;
	return stolen;
}

int CritterGroupGenerator::deleteCritter(int critterIndex) {
	// Only removes the critter. The reward / theft is counted by moveAlongPath()
	// before it calls this.
	if (critterIndex >= 0 && critterIndex < static_cast<int>(activeCritters.size())) {
		activeCritters.erase(activeCritters.begin() + critterIndex);
	}
	return 0;
}


int CritterGroupGenerator::activeCrittersLeft() {
	return static_cast<int>(activeCritters.size());
}

std::vector<Critter*> CritterGroupGenerator::getActiveCritterPointers() {
	std::vector<Critter*> pointers;
	pointers.reserve(activeCritters.size()); // one slot per critter, no regrowing

	// Take each critter by reference (Critter&, not a copy) so &critter is the
	// address of the critter inside activeCritters. Towers then damage that one.
	for (Critter& critter : activeCritters) {
		pointers.push_back(&critter);
	}
	return pointers;
}
