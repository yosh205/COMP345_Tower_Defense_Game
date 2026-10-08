/**
 * @file Critter.h
 * @brief Minimal critter for tower targeting, damage, and slow effects.
 *
 * A fuller wave/movement system belongs to later parts; this is enough for
 * towers to detect range, pick a target, and apply their shot effects.
 */
#ifndef Critter_h
#define Critter_h

/** @brief Kind of critter; some towers deal bonus damage to certain kinds. */
enum class CritterKind {
    Normal,  ///< No special resistances or weaknesses.
    Armored, ///< Extra hit points; takes bonus damage from direct towers.
    Fast     ///< Higher base speed; more affected by slow towers.
};

/**
 * @brief An enemy that walks the path and can be damaged or slowed by towers.
 */
class Critter {
public:
    /**
     * @param kind      Critter kind (affects speed and some tower bonuses).
     * @param maxHitPoints Starting / maximum hit points.
     * @param baseSpeed Cells per second while not slowed.
     * @param row       Starting row (usually the entry).
     * @param col       Starting column (usually the entry).
     * @param level     Difficulty level, 1 or higher (values below 1 become 1).
     *                  The reward and strength are calculated from it.
     */
    Critter(CritterKind kind, int maxHitPoints, float baseSpeed, int row, int col,
            int level = 1);

    CritterKind getKind() const;
    int getHitPoints() const;
    int getMaxHitPoints() const;
    bool isAlive() const;

    /** @return Current movement speed (base speed after slow). */
    float getSpeed() const;
    float getBaseSpeed() const;

    float getRow() const;
    float getCol() const;
    void setPosition(float row, float col);

    // these two functions allow us to add fractional progress to a critter's movement.
    float getProgress();
    void addProgress(float cellsMoved);

    /**
     * @brief How far along the path this critter is (0 = entry).
     * Towers that prefer "first" targets use the highest index in range.
     */
    int getPathIndex() const;
    void setPathIndex(int index);

    /** @brief Reduces hit points; clamps at 0. */
    void takeDamage(int amount);

    /**
     * @brief Multiplies speed by factor for durationSeconds.
     * Stronger slows (lower factor) replace weaker ones; durations refresh.
     */
    void applySlow(float factor, float durationSeconds);

    /** @brief Advances slow timers. Call once per frame with frame dt. */
    void updateEffects(float dt);

    /** @brief Euclidean distance in cells from this critter to (row, col). */
    float distanceTo(int row, int col) const;

    /** @return Difficulty level of this critter (1 = easiest). */
    int getLevel() const;

    /**
     * @return Coins the player earns for killing this critter.
     * Game rule: the reward is proportional to the level (5 coins per level),
     * plus 50% for Armored critters because they are harder to kill.
     */
    int getReward() const;

    /**
     * @return How dangerous this critter is when it reaches the exit.
     * Equal to its level, plus 1 for Fast critters because they are harder to stop.
     */
    int getStrength() const;

    /**
     * @return Coins stolen from the player when this critter reaches the exit.
     * Game rule: determined by strength (3 coins per point of strength).
     */
    int getCoinsStolen() const;

private:
    CritterKind kind;
    int maxHitPoints;
    int hitPoints;
    float baseSpeed;
    float slowFactor;     ///< 1 = normal; < 1 = slowed.
    float slowTimeLeft;   ///< Seconds remaining on the current slow.
    float row;
    float col;
    int pathIndex;
    float progress = 0.f;
    int level;     ///< Difficulty level, 1 or higher.
    int reward;    ///< Coins earned when killed (set from level and kind).
    int strength;  ///< Sets the coins stolen at the exit (set from level and kind).
};

#endif // Critter_h
