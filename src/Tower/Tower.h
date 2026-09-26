/**
 * @file Tower.h
 * @brief Base tower: cost, levels, range, fire rate, and target selection.
 *
 * Subclasses differ mainly in fireAt() behaviour (direct, area, slow, …).
 */
#ifndef Tower_h
#define Tower_h

#include <string>
#include <vector>

#include "../Critter/Critter.h"

/**
 * @brief Abstract tower placed on a scenery cell.
 *
 * Buying cost, refund value, range, power and rate of fire scale with level.
 * update() handles cooldown, finds targets in range, selects one, and shoots.
 */
class Tower {
public:
    virtual ~Tower() = default;

    /** @return Short name for UI / logging (e.g. "Direct", "Area", "Slow"). */
    virtual std::string getTypeName() const = 0;

    int getRow() const;
    int getCol() const;
    int getLevel() const;
    int getMaxLevel() const;

    /** @return Cost paid to buy this tower at level 1. */
    int getBuyCost() const;

    /** @return Cost to upgrade from the current level to the next, or 0 if max. */
    int getUpgradeCost() const;

    /** @return Sell refund for the current level (portion of money spent). */
    int getRefundValue() const;

    float getRange() const;
    int getPower() const;

    /** @return Shots per second at the current level. */
    float getFireRate() const;

    bool canUpgrade() const;

    /**
     * @brief Pays upgradeCost (caller handles gold) and raises level by one.
     * @return true if upgraded, false if already at max level.
     */
    bool upgrade();

    /**
     * @brief Each frame: tick cooldown, then detect / select / shoot.
     * @param dt       Seconds since last frame.
     * @param critters Living and dead critters; dead ones are skipped.
     */
    void update(float dt, std::vector<Critter*>& critters);

    /** @return Critters currently within range (alive only). */
    std::vector<Critter*> detectTargets(const std::vector<Critter*>& critters) const;

protected:
    /**
     * @param row           Placement row (scenery cell).
     * @param col           Placement column.
     * @param buyCost       Purchase price at level 1.
     * @param baseRange     Range in cells at level 1.
     * @param basePower     Damage (or effect strength) at level 1.
     * @param baseFireRate  Shots per second at level 1.
     * @param maxLevel      Highest level (inclusive), at least 1.
     */
    Tower(int row, int col, int buyCost, float baseRange, int basePower,
          float baseFireRate, int maxLevel);

    /**
     * @brief Picks which detected critter to shoot.
     * Default: furthest along the path ("first" / closest to exit).
     */
    virtual Critter* selectTarget(const std::vector<Critter*>& inRange) const;

    /**
     * @brief Apply this tower's shot effect to the chosen target.
     * @param target     Selected critter (guaranteed alive and in range).
     * @param allCritters Full list (area towers may hit neighbours too).
     */
    virtual void fireAt(Critter& target, std::vector<Critter*>& allCritters) = 0;

    /** @brief Recalculates range, power and fire rate from level. */
    void refreshStats();

    int row;
    int col;
    int level;
    int maxLevel;
    int buyCost;
    int totalSpent;   ///< Buy + all upgrades so far (basis for refund).
    float baseRange;
    int basePower;
    float baseFireRate;
    float range;
    int power;
    float fireRate;
    float cooldown;   ///< Seconds until the next shot is allowed.
};

#endif // Tower_h
