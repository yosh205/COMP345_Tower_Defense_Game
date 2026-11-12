/**
 * @file TowerDecorator.h
 * @brief Assignment 3, Part 3 (Decorator pattern): changing what a tower does as it is upgraded.
 *
 * A decorator wraps a tower and is itself a Tower, so the game keeps a
 * std::unique_ptr<Tower> and wraps it again on each upgrade or special effect:
 *
 *     towers[i] = std::make_unique<SplashDecorator>(std::move(towers[i]));
 *
 * Stats, cell, strategy and observers stay on the wrapped tower; the outermost
 * tower's update() uses virtual getters and fireAt() so splash / burn / freeze apply.
 */
#ifndef TowerDecorator_h
#define TowerDecorator_h

#include <memory>
#include <string>

#include "Tower.h"

/**
 * @brief Base decorator: a Tower that wraps another Tower and forwards to it.
 *
 * Concrete decorators override only what they change, and call the wrapped
 * tower for everything else.
 */
class TowerDecorator : public Tower {
public:
    /**
     * @brief Wraps a tower.
     * @param wrapped The tower to decorate. The decorator owns it.
     */
    explicit TowerDecorator(std::unique_ptr<Tower> wrapped);

    std::string getTypeName() const override;

    int getRow() const override;
    int getCol() const override;
    int getLevel() const override;
    int getMaxLevel() const override;
    int getBuyCost() const override;
    int getUpgradeCost() const override;
    int getRefundValue() const override;
    float getRange() const override;
    int getPower() const override;
    float getFireRate() const override;

    std::string getStrategy() const override;
    void setStrategy(std::unique_ptr<TargetingStrategy> newStrategy) override;
    void cycleStrategies() override;

    bool canUpgrade() const override;
    bool upgrade() override;

    Critter* selectTarget(const std::vector<Critter*>& inRange) const override;
    void fireAt(Critter& target, std::vector<Critter*>& allCritters) override;

    bool hasEffect(const std::string& tag) const override;
    std::string getEffectTags() const override;
    void addInvestment(int gold) override;

protected:
    std::unique_ptr<Tower> inner;  ///< The decorated tower.

    /** @return Tag for this decorator layer, or empty if it should not be listed. */
    virtual std::string effectTag() const;
};

/** @brief Raises the wrapped tower one level (replaces Tower::upgrade() in the game). */
class LevelUpDecorator : public TowerDecorator {
public:
    /** @param wrapped The tower to upgrade. Must still be below max level. */
    explicit LevelUpDecorator(std::unique_ptr<Tower> wrapped);
};

/** @brief Splash: the damage also hits critters near the target. */
class SplashDecorator : public TowerDecorator {
public:
    /** @param wrapped The tower that gets splash damage. */
    explicit SplashDecorator(std::unique_ptr<Tower> wrapped);
    void fireAt(Critter& target, std::vector<Critter*>& allCritters) override;

protected:
    std::string effectTag() const override;
};

/** @brief Burning: the target takes damage again over a few seconds after a hit. */
class BurningDecorator : public TowerDecorator {
public:
    /** @param wrapped The tower that gets burning damage. */
    explicit BurningDecorator(std::unique_ptr<Tower> wrapped);
    void fireAt(Critter& target, std::vector<Critter*>& allCritters) override;

protected:
    std::string effectTag() const override;
};

/** @brief Freezing: a hit also slows the target down. */
class FreezingDecorator : public TowerDecorator {
public:
    /** @param wrapped The tower that gets freezing damage. */
    explicit FreezingDecorator(std::unique_ptr<Tower> wrapped);
    void fireAt(Critter& target, std::vector<Critter*>& allCritters) override;

protected:
    std::string effectTag() const override;
};

#endif // TowerDecorator_h
