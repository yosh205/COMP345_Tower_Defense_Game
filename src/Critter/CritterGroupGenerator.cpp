/**
 * @file CritterGroupGenerator.cpp
 * @brief Implements CritterGroupGenerator. Function docs are in CritterGroupGenerator.h.
 */
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include "CritterGroupGenerator.h"
#include "../Critter/Critter.h"
#include "../Map/Map.h"
#include <iostream>
#include "CritterFactory.h"

namespace {

/**
 * @brief Puts critter at progress along path, using the same interpolation as movement.
 * progress is clamped to the path. The position is updated to match.
 */
void placeAlong(Critter& critter, const std::vector<Position>& path, float progress) {
	const int last = static_cast<int>(path.size()) - 1;
	if (last < 0) {
		return;
	}
	if (progress < 0.f) {
		progress = 0.f;
	}
	int index = static_cast<int>(progress);
	if (index >= last) {
		index = last;
		progress = static_cast<float>(last);
	}
	const float t = progress - static_cast<float>(index);
	const Position current = path[static_cast<std::size_t>(index)];
	const Position next = path[static_cast<std::size_t>(std::min(index + 1, last))];
	critter.setProgress(progress);
	critter.setPathIndex(index);
	critter.setPosition(current.row + t * (next.row - current.row),
	                    current.col + t * (next.col - current.col));
}

} // namespace

CritterGroupGenerator::CritterGroupGenerator(int waveNb) { // for now, hardcoded to always generate 20 critters. Can be refined to have a percentage-based implementation
	// Every critter's level is the wave number, so its reward and strength
	// (calculated by Critter from the level) grow with each wave.
	//Normal Critters
	for (int i = 0; i < 10; i++) {
		Critter newCritter = CritterFactory::createCritter(CritterKind::Normal, waveNb);
		addCritterToGroup(newCritter);
	}
	//Armored Critters
	for (int i = 0; i < 5; i++) {
		Critter newCritter = CritterFactory::createCritter(CritterKind::Armored, waveNb);
		addCritterToGroup(newCritter);
	}
	//Fast Critters
	for (int i = 0; i < 5; i++) {
		Critter newCritter = CritterFactory::createCritter(CritterKind::Fast, waveNb);
		addCritterToGroup(newCritter);
	}
	//create a group of critters, how do we determine the amount/kind of critters?
}

void CritterGroupGenerator::setAllowOverlap(bool allow) {
	allowOverlap = allow;
}

bool CritterGroupGenerator::allowsOverlap() const {
	return allowOverlap;
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
			// It got through: it steals coins based on its strength, and costs half a heart.
			coinsStolen += critter.getCoinsStolen();
			++crittersEscaped;
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

	// One critter per tile: walk from the front of the line backward and stop
	// anyone who has stepped onto a tile that the critter in front still holds.
	if (!allowOverlap && activeCritters.size() > 1) {
		std::vector<std::size_t> order(activeCritters.size());
		for (std::size_t i = 0; i < order.size(); ++i) {
			order[i] = i;
		}
		std::sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b) {
			const float progressA = activeCritters[a].getProgress();
			const float progressB = activeCritters[b].getProgress();
			if (progressA != progressB) {
				return progressA > progressB;
			}
			return a < b; // the one that entered earlier keeps the tile
		});
		for (std::size_t n = 1; n < order.size(); ++n) {
			// Keep a full cell between critters, so they never touch on screen.
			const float limit = activeCritters[order[n - 1]].getProgress() - 1.f;
			Critter& behind = activeCritters[order[n]];
			if (behind.getProgress() > limit) {
				placeAlong(behind, path, std::max(0.f, limit));
			}
		}
	}
}

bool CritterGroupGenerator::findNextCritter(const std::vector<Position> path) {

	if (pendingCritters.empty()) {
		return false;
	}
	if (!allowOverlap) {
		for (Critter& critter : activeCritters) {
			if (critter.isAlive() && static_cast<int>(critter.getProgress()) <= 0) {
				return false; // the entry tile is still occupied
			}
		}
	}
	Critter nextCritter = pendingCritters.front();
	pendingCritters.pop();
	nextCritter.setPathIndex(0);
	nextCritter.setPosition(path[0].row, path[0].col);
	activeCritters.push_back(nextCritter);
	return true;
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

int CritterGroupGenerator::collectCrittersEscaped() {
	const int escaped = crittersEscaped;
	crittersEscaped = 0;
	return escaped;
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

int CritterGroupGenerator::crittersRemaining() const {
	return static_cast<int>(pendingCritters.size() + activeCritters.size());
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
