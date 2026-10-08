/**
 * @file tests.cpp
 * @brief Test cases for the Map, Tower and Critter parts of Assignment 1.
 *
 * Each test function checks one game rule from the assignment. The program
 * prints PASS or FAIL for every check and returns a non-zero exit code if any
 * check fails. It does not use SFML, so it runs without a screen.
 */
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Critter/Critter.h"
#include "Critter/CritterGroupGenerator.h"
#include "Map/Map.h"
#include "Tower/AreaDamageTower.h"
#include "Tower/DirectDamageTower.h"
#include "Tower/SlowingTower.h"

namespace {

int failures = 0;
int checks = 0;

/** @brief Prints PASS or FAIL for one condition and counts failures. */
#define CHECK(cond)                                                          \
    do {                                                                     \
        ++checks;                                                            \
        if (cond) {                                                          \
            std::cout << "  PASS: " #cond "\n";                              \
        } else {                                                             \
            std::cout << "  FAIL: " #cond "  (line " << __LINE__ << ")\n";   \
            ++failures;                                                      \
        }                                                                    \
    } while (0)

/** @brief Checks that an expression throws std::invalid_argument. */
#define CHECK_THROWS(expr)                                                   \
    do {                                                                     \
        bool threw = false;                                                  \
        try {                                                                \
            expr;                                                            \
        } catch (const std::invalid_argument&) {                             \
            threw = true;                                                    \
        }                                                                    \
        CHECK(threw && "throws: " #expr);                                    \
    } while (0)

const float kFrame = 1.f / 60.f;  ///< One frame at 60 frames per second.

/** @brief Builds a valid 3x5 map: Entry at (1,0), path along row 1, Exit at (1,4). */
Map makeStraightMap() {
    Map map(3, 5);
    map.setCell(1, 0, CellType::Entry);
    for (int c = 1; c <= 3; ++c) {
        map.setCell(1, c, CellType::Path);
    }
    map.setCell(1, 4, CellType::Exit);
    return map;
}

/** @brief Moves every critter waiting in the wave onto the map. */
void spawnAll(CritterGroupGenerator& wave, const std::vector<Position>& path) {
    while (wave.crittersRemaining() > wave.activeCrittersLeft()) {
        wave.findNextCritter(path);
    }
}

// ---------------------------------------------------------------- Part 1: Map

void testMapCreation() {
    std::cout << "\n[Map] A blank map of the requested size is all scenery\n";
    Map map(4, 6);
    CHECK(map.getLength() == 4);
    CHECK(map.getWidth() == 6);
    CHECK(map.getCellType(3, 5) == CellType::Scenery);
    CHECK(!map.isValid());  // no entry and no exit yet
    CHECK_THROWS(Map(0, 5));
    CHECK_THROWS(Map(5, -1));
}

void testMapSetters() {
    std::cout << "\n[Map] Any cell can be set, but only inside the map\n";
    Map map(3, 3);
    CHECK(map.setCell(2, 2, CellType::Path));
    CHECK(map.getCellType(2, 2) == CellType::Path);
    CHECK(!map.setCell(3, 0, CellType::Path));   // row out of bounds
    CHECK(!map.setCell(0, -1, CellType::Path));  // column out of bounds
}

void testMapValidity() {
    std::cout << "\n[Map] Valid only with one entry, one exit and a connected path\n";
    CHECK(makeStraightMap().isValid());

    Map noExit = makeStraightMap();
    noExit.setCell(1, 4, CellType::Path);
    CHECK(!noExit.isValid());

    Map twoEntries = makeStraightMap();
    twoEntries.setCell(0, 0, CellType::Entry);
    CHECK(!twoEntries.isValid());

    Map twoExits = makeStraightMap();
    twoExits.setCell(2, 4, CellType::Exit);
    CHECK(!twoExits.isValid());

    Map gap = makeStraightMap();
    gap.setCell(1, 2, CellType::Scenery);
    CHECK(!gap.isValid());

    // Diagonal cells are not connected: critters move up/down/left/right only.
    Map diagonal(2, 2);
    diagonal.setCell(0, 0, CellType::Entry);
    diagonal.setCell(1, 1, CellType::Exit);
    CHECK(!diagonal.isValid());
}

void testMapPath() {
    std::cout << "\n[Map] findPath returns the route from entry to exit\n";
    std::vector<Position> path = makeStraightMap().findPath();
    CHECK(path.size() == 5);
    CHECK(path.front().row == 1 && path.front().col == 0);  // entry first
    CHECK(path.back().row == 1 && path.back().col == 4);    // exit last

    Map gap = makeStraightMap();
    gap.setCell(1, 2, CellType::Scenery);
    CHECK(gap.findPath().empty());
}

void testMapTowerPlacement() {
    std::cout << "\n[Map] Towers go on scenery; critters walk on path cells\n";
    Map map = makeStraightMap();
    CHECK(map.canPlaceTower(0, 0));   // scenery
    CHECK(!map.canPlaceTower(1, 2));  // path
    CHECK(!map.canPlaceTower(1, 0));  // entry
    CHECK(!map.canPlaceTower(9, 9));  // outside the map
    CHECK(map.isWalkable(1, 0) && map.isWalkable(1, 2) && map.isWalkable(1, 4));
    CHECK(!map.isWalkable(0, 0));
}

// ------------------------------------------------------------- Part 2: Towers

void testTowerLevels() {
    std::cout << "\n[Tower] Upgrading raises the level and capacities, up to the max level\n";
    DirectDamageTower tower(0, 0);
    const int power1 = tower.getPower();
    const float range1 = tower.getRange();
    CHECK(tower.getLevel() == 1);
    CHECK(tower.getUpgradeCost() > 0);

    CHECK(tower.upgrade());
    CHECK(tower.getLevel() == 2);
    CHECK(tower.getPower() > power1);
    CHECK(tower.getRange() > range1);

    while (tower.canUpgrade()) {
        tower.upgrade();
    }
    CHECK(tower.getLevel() == tower.getMaxLevel());
    CHECK(!tower.upgrade());  // cannot go past the max level
    CHECK(tower.getUpgradeCost() == 0);
}

void testTowerSelling() {
    std::cout << "\n[Tower] Selling refunds part of the money spent, more at higher levels\n";
    DirectDamageTower tower(0, 0);
    const int refund1 = tower.getRefundValue();
    CHECK(refund1 > 0 && refund1 < tower.getBuyCost());
    tower.upgrade();
    CHECK(tower.getRefundValue() > refund1);
}

void testTowerTargeting() {
    std::cout << "\n[Tower] Detects critters in range, then shoots the one furthest along\n";
    DirectDamageTower tower(0, 0);  // range 2.5 cells at level 1
    Critter near(CritterKind::Normal, 100, 1.f, 0, 1);
    Critter ahead(CritterKind::Normal, 100, 1.f, 0, 2);
    Critter far(CritterKind::Normal, 100, 1.f, 0, 9);
    near.setPathIndex(1);
    ahead.setPathIndex(2);
    far.setPathIndex(9);
    std::vector<Critter*> critters = {&near, &ahead, &far};

    CHECK(tower.detectTargets(critters).size() == 2);  // 'far' is out of range

    tower.update(0.f, critters);
    CHECK(ahead.getHitPoints() < 100);  // furthest along in range is shot
    CHECK(near.getHitPoints() == 100);
    CHECK(far.getHitPoints() == 100);

    // Rate of fire: right after shooting, the tower waits for its cooldown.
    const int hpAfterFirstShot = ahead.getHitPoints();
    tower.update(0.01f, critters);
    CHECK(ahead.getHitPoints() == hpAfterFirstShot);
}

void testTowerTypes() {
    std::cout << "\n[Tower] The three tower types behave differently\n";

    // Direct: bonus damage against armored critters.
    DirectDamageTower directA(0, 0);
    DirectDamageTower directB(0, 0);
    Critter normal(CritterKind::Normal, 100, 1.f, 0, 1);
    Critter armored(CritterKind::Armored, 100, 1.f, 0, 1);
    std::vector<Critter*> onlyNormal = {&normal};
    std::vector<Critter*> onlyArmored = {&armored};
    directA.update(0.f, onlyNormal);
    directB.update(0.f, onlyArmored);
    CHECK(100 - armored.getHitPoints() > 100 - normal.getHitPoints());

    // Area: the target and a critter next to it are both damaged.
    AreaDamageTower area(0, 0);
    Critter target(CritterKind::Normal, 100, 1.f, 0, 1);
    Critter neighbour(CritterKind::Normal, 100, 1.f, 1, 1);
    Critter distant(CritterKind::Normal, 100, 1.f, 0, 20);
    target.setPathIndex(5);
    std::vector<Critter*> group = {&target, &neighbour, &distant};
    area.update(0.f, group);
    CHECK(target.getHitPoints() < 100);
    CHECK(neighbour.getHitPoints() < 100);
    CHECK(neighbour.getHitPoints() > target.getHitPoints());  // splash is weaker
    CHECK(distant.getHitPoints() == 100);

    // Slow: the target moves slower for a while, then recovers.
    SlowingTower slow(0, 0);
    Critter runner(CritterKind::Normal, 100, 2.f, 0, 1);
    std::vector<Critter*> onlyRunner = {&runner};
    slow.update(0.f, onlyRunner);
    CHECK(runner.getSpeed() < runner.getBaseSpeed());
    runner.updateEffects(slow.getSlowDuration() + 0.1f);
    CHECK(runner.getSpeed() == runner.getBaseSpeed());
}

// ----------------------------------------------------------- Part 3: Critters

void testCritterStats() {
    std::cout << "\n[Critter] Reward is proportional to level; strength sets coins stolen\n";
    Critter level1(CritterKind::Normal, 10, 1.f, 0, 0, 1);
    Critter level4(CritterKind::Normal, 10, 1.f, 0, 0, 4);
    CHECK(level4.getReward() == 4 * level1.getReward());
    CHECK(level4.getStrength() > level1.getStrength());
    CHECK(level4.getCoinsStolen() > level1.getCoinsStolen());

    Critter armored(CritterKind::Armored, 10, 1.f, 0, 0, 4);
    Critter fast(CritterKind::Fast, 10, 1.f, 0, 0, 4);
    CHECK(armored.getReward() > level4.getReward());      // armored worth more
    CHECK(fast.getStrength() > level4.getStrength());     // fast steals more

    Critter defaultLevel(CritterKind::Normal, 10, 1.f, 0, 0);
    Critter badLevel(CritterKind::Normal, 10, 1.f, 0, 0, 0);
    CHECK(defaultLevel.getLevel() == 1);
    CHECK(badLevel.getLevel() == 1);  // levels below 1 become 1
}

void testCritterDamage() {
    std::cout << "\n[Critter] Towers reduce hit points; the critter dies at zero\n";
    Critter critter(CritterKind::Normal, 30, 1.f, 0, 0);
    critter.takeDamage(10);
    CHECK(critter.getHitPoints() == 20);
    CHECK(critter.isAlive());
    critter.takeDamage(50);
    CHECK(critter.getHitPoints() == 0);  // never below zero
    CHECK(!critter.isAlive());
}

void testWaveCreation() {
    std::cout << "\n[Critter waves] A wave is a group of critters adapted to its difficulty\n";
    const std::vector<Position> path = makeStraightMap().findPath();
    CritterGroupGenerator wave1(1);
    CritterGroupGenerator wave3(3);

    CHECK(wave1.crittersRemaining() == 20);
    CHECK(wave1.activeCrittersLeft() == 0);  // nobody on the map before spawning

    spawnAll(wave1, path);
    spawnAll(wave3, path);
    const Critter& firstOfWave1 = wave1.getActiveCritters().front();
    const Critter& firstOfWave3 = wave3.getActiveCritters().front();

    bool allLevel3 = true;
    for (const Critter& c : wave3.getActiveCritters()) {
        allLevel3 = allLevel3 && c.getLevel() == 3;
    }
    CHECK(allLevel3);                                                    // level = wave number
    CHECK(firstOfWave3.getMaxHitPoints() > firstOfWave1.getMaxHitPoints());
    CHECK(firstOfWave3.getBaseSpeed() > firstOfWave1.getBaseSpeed());
    CHECK(firstOfWave3.getReward() > firstOfWave1.getReward());
}

void testWaveSpawning() {
    std::cout << "\n[Critter waves] Critters appear one after the other at the entry\n";
    const std::vector<Position> path = makeStraightMap().findPath();
    CritterGroupGenerator wave(1);

    wave.findNextCritter(path);
    CHECK(wave.activeCrittersLeft() == 1);
    CHECK(wave.crittersRemaining() == 20);  // 1 on the map + 19 waiting
    const Critter& first = wave.getActiveCritters().front();
    CHECK(first.getRow() == path.front().row && first.getCol() == path.front().col);

    wave.findNextCritter(path);
    CHECK(wave.activeCrittersLeft() == 2);
}

void testWaveMovement() {
    std::cout << "\n[Critter waves] Critters move toward the exit at their speed, never back\n";
    const std::vector<Position> path = makeStraightMap().findPath();  // 4 cells long
    CritterGroupGenerator wave(1);
    wave.findNextCritter(path);
    const float speed = wave.getActiveCritters().front().getSpeed();
    const float expectedSeconds = static_cast<float>(path.size() - 1) / speed;

    float seconds = 0.f;
    int lastIndex = 0;
    bool neverBackwards = true;
    while (wave.activeCrittersLeft() > 0 && seconds < 60.f) {
        wave.moveAlongPath(path, kFrame);
        seconds += kFrame;
        if (wave.activeCrittersLeft() > 0) {
            const int index = wave.getActiveCritters().front().getPathIndex();
            neverBackwards = neverBackwards && index >= lastIndex;
            lastIndex = index;
        }
    }
    CHECK(neverBackwards);
    CHECK(wave.activeCrittersLeft() == 0);  // removed once it reached the exit
    CHECK(seconds > expectedSeconds - 0.1f && seconds < expectedSeconds + 0.1f);
}

void testWaveCoins() {
    std::cout << "\n[Critter waves] Killed critters pay a reward; escaped ones steal coins\n";
    const std::vector<Position> path = makeStraightMap().findPath();

    // Escape: a critter that reaches the exit steals getCoinsStolen() coins.
    CritterGroupGenerator escaping(2);
    escaping.findNextCritter(path);
    const int toSteal = escaping.getActiveCritters().front().getCoinsStolen();
    for (int i = 0; i < 60 * 60 && escaping.activeCrittersLeft() > 0; ++i) {
        escaping.moveAlongPath(path, kFrame);
    }
    CHECK(escaping.collectCoinsStolen() == toSteal);
    CHECK(escaping.collectCoinsEarned() == 0);
    CHECK(escaping.collectCoinsStolen() == 0);  // collecting resets the count

    // Kill: a critter with no hit points pays getReward() coins and is removed.
    CritterGroupGenerator killed(2);
    killed.findNextCritter(path);
    Critter* victim = killed.getActiveCritterPointers().front();
    const int reward = victim->getReward();
    victim->takeDamage(victim->getMaxHitPoints());
    killed.moveAlongPath(path, kFrame);
    CHECK(killed.activeCrittersLeft() == 0);
    CHECK(killed.collectCoinsEarned() == reward);
    CHECK(killed.collectCoinsStolen() == 0);

    // A critter killed right next to the exit pays its reward and never steals,
    // even if this frame's movement would have carried it onto the exit.
    CritterGroupGenerator lastSecond(2);
    lastSecond.findNextCritter(path);
    for (int i = 0; i < 60 * 60; ++i) {
        lastSecond.moveAlongPath(path, kFrame);
        if (lastSecond.getActiveCritters().front().getPathIndex() >= 3) {
            break;  // one cell before the exit
        }
    }
    Critter* nearExit = lastSecond.getActiveCritterPointers().front();
    const int nearExitReward = nearExit->getReward();  // read now: the critter is removed below
    nearExit->takeDamage(nearExit->getMaxHitPoints());
    lastSecond.moveAlongPath(path, 10.f);  // a huge step that would pass the exit
    CHECK(lastSecond.collectCoinsStolen() == 0);
    CHECK(lastSecond.collectCoinsEarned() == nearExitReward);
}

void testWaveEnds() {
    std::cout << "\n[Critter waves] The wave is over once every critter is killed or escaped\n";
    const std::vector<Position> path = makeStraightMap().findPath();
    CritterGroupGenerator wave(1);
    spawnAll(wave, path);
    for (Critter* c : wave.getActiveCritterPointers()) {
        c->takeDamage(c->getMaxHitPoints());
    }
    wave.moveAlongPath(path, kFrame);
    CHECK(wave.crittersRemaining() == 0);
    CHECK(wave.collectCoinsEarned() > 0);
}

}  // namespace

int main() {
    std::cout << "Tower Defense - Assignment 1 tests\n";

    testMapCreation();
    testMapSetters();
    testMapValidity();
    testMapPath();
    testMapTowerPlacement();

    testTowerLevels();
    testTowerSelling();
    testTowerTargeting();
    testTowerTypes();

    testCritterStats();
    testCritterDamage();
    testWaveCreation();
    testWaveSpawning();
    testWaveMovement();
    testWaveCoins();
    testWaveEnds();

    std::cout << "\n" << (checks - failures) << " / " << checks << " checks passed\n";
    return failures == 0 ? 0 : 1;
}
