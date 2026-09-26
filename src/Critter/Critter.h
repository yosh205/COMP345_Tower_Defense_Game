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
     */
    Critter(CritterKind kind, int maxHitPoints, float baseSpeed, int row, int col);

    CritterKind getKind() const;
    int getHitPoints() const;
    int getMaxHitPoints() const;
    bool isAlive() const;

    /** @return Current movement speed (base speed after slow). */
    float getSpeed() const;
    float getBaseSpeed() const;

    int getRow() const;
    int getCol() const;
    void setPosition(int row, int col);

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

private:
    CritterKind kind;
    int maxHitPoints;
    int hitPoints;
    float baseSpeed;
    float slowFactor;     ///< 1 = normal; < 1 = slowed.
    float slowTimeLeft;   ///< Seconds remaining on the current slow.
    int row;
    int col;
    int pathIndex;
};

#endif // Critter_h
