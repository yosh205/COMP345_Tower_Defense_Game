/**
 * @file Critter.h
 * @brief Declares Critter: an enemy with hit points, speed, level, reward and strength.
 *
 * Critters are created in waves by CritterGroupGenerator, which also moves them
 * along the path. Towers damage and slow them through takeDamage() and applySlow().
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
 *
 * Game rules: a critter dies when its hit points reach 0 and then pays its
 * reward (proportional to its level). If it reaches the exit first, it steals
 * coins according to its strength.
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

    /** @return The kind of critter (Normal, Armored or Fast). */
    CritterKind getKind() const;
    /** @return Hit points left (0 means dead). */
    int getHitPoints() const;
    /** @return Hit points the critter started with. */
    int getMaxHitPoints() const;
    /** @return True while the critter has more than 0 hit points. */
    bool isAlive() const;

    /** @return Current movement speed (base speed after slow). */
    float getSpeed() const;
    /** @return Speed in cells per second when not slowed. */
    float getBaseSpeed() const;

    /** @return Current row; fractional while the critter is between two cells. */
    float getRow() const;
    /** @return Current column; fractional while the critter is between two cells. */
    float getCol() const;
    /** @brief Places the critter at (row, col) on the map, in cells. */
    void setPosition(float row, float col);

    /**
     * @return Distance travelled along the path, in cells (e.g. 2.5 = halfway
     *         between the 3rd and 4th path cells). Used for smooth movement.
     */
    float getProgress();
    /** @brief Adds cellsMoved to the distance travelled along the path. */
    void addProgress(float cellsMoved);

    /**
     * @brief How far along the path this critter is (0 = entry).
     * Towers that prefer "first" targets use the highest index in range.
     */
    int getPathIndex() const;
    /** @brief Sets the index of the path cell the critter is on (0 = entry). */
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
    CritterKind kind;     ///< Normal, Armored or Fast.
    int maxHitPoints;     ///< Starting hit points.
    int hitPoints;        ///< Hit points left; 0 = dead.
    float baseSpeed;      ///< Cells per second when not slowed.
    float slowFactor;     ///< 1 = normal; < 1 = slowed.
    float slowTimeLeft;   ///< Seconds remaining on the current slow.
    float row;            ///< Current row (fractional between cells).
    float col;            ///< Current column (fractional between cells).
    int pathIndex;        ///< Index of the path cell the critter is on (0 = entry).
    float progress = 0.f; ///< Cells travelled along the path.
    int level;     ///< Difficulty level, 1 or higher.
    int reward;    ///< Coins earned when killed (set from level and kind).
    int strength;  ///< Sets the coins stolen at the exit (set from level and kind).
};

#endif // Critter_h
