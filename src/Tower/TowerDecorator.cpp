/**
 * @file TowerDecorator.cpp
 * @brief Implements the tower decorators. Function docs are in TowerDecorator.h.
 */
#include "TowerDecorator.h"

#include <algorithm>

namespace {
constexpr float kSplashRadius = 1.25f;
constexpr float kBurnDps = 8.f;
constexpr float kBurnDuration = 3.f;
constexpr float kFreezeFactor = 0.55f;
constexpr float kFreezeDuration = 2.0f;
} // namespace

TowerDecorator::TowerDecorator(std::unique_ptr<Tower> wrapped)
    : Tower(wrapped->getRow(),
            wrapped->getCol(),
            wrapped->getBuyCost(),
            wrapped->getRange(),
            wrapped->getPower(),
            wrapped->getFireRate(),
            wrapped->getMaxLevel()),
      inner(std::move(wrapped)) {}

std::string TowerDecorator::getTypeName() const { return inner->getTypeName(); }

int TowerDecorator::getRow() const { return inner->getRow(); }
int TowerDecorator::getCol() const { return inner->getCol(); }
int TowerDecorator::getLevel() const { return inner->getLevel(); }
int TowerDecorator::getMaxLevel() const { return inner->getMaxLevel(); }
int TowerDecorator::getBuyCost() const { return inner->getBuyCost(); }
int TowerDecorator::getUpgradeCost() const { return inner->getUpgradeCost(); }
int TowerDecorator::getRefundValue() const { return inner->getRefundValue(); }
float TowerDecorator::getRange() const { return inner->getRange(); }
int TowerDecorator::getPower() const { return inner->getPower(); }
float TowerDecorator::getFireRate() const { return inner->getFireRate(); }

std::string TowerDecorator::getStrategy() const { return inner->getStrategy(); }

void TowerDecorator::setStrategy(std::unique_ptr<TargetingStrategy> newStrategy) {
    inner->setStrategy(std::move(newStrategy));
}

void TowerDecorator::cycleStrategies() { inner->cycleStrategies(); }

bool TowerDecorator::canUpgrade() const { return inner->canUpgrade(); }

bool TowerDecorator::upgrade() { return inner->upgrade(); }

Critter* TowerDecorator::selectTarget(const std::vector<Critter*>& inRange) const {
    return inner->selectTarget(inRange);
}

void TowerDecorator::fireAt(Critter& target, std::vector<Critter*>& allCritters) {
    inner->fireAt(target, allCritters);
}

bool TowerDecorator::hasEffect(const std::string& tag) const {
    if (!effectTag().empty() && effectTag() == tag) {
        return true;
    }
    return inner->hasEffect(tag);
}

std::string TowerDecorator::getEffectTags() const {
    const std::string own = effectTag();
    const std::string rest = inner->getEffectTags();
    if (own.empty()) {
        return rest;
    }
    if (rest.empty()) {
        return own;
    }
    return own + " " + rest;
}

void TowerDecorator::addInvestment(int gold) { inner->addInvestment(gold); }

std::string TowerDecorator::effectTag() const { return ""; }

LevelUpDecorator::LevelUpDecorator(std::unique_ptr<Tower> wrapped)
    : TowerDecorator(std::move(wrapped)) {
    // Apply the level bump on the wrapped tower; this decorator records the upgrade.
    inner->upgrade();
}

SplashDecorator::SplashDecorator(std::unique_ptr<Tower> wrapped)
    : TowerDecorator(std::move(wrapped)) {}

std::string SplashDecorator::effectTag() const { return "SPLASH"; }

void SplashDecorator::fireAt(Critter& target, std::vector<Critter*>& allCritters) {
    TowerDecorator::fireAt(target, allCritters);

    const float tr = target.getRow();
    const float tc = target.getCol();
    const int splashDamage = std::max(1, getPower() / 2);
    for (Critter* c : allCritters) {
        if (c == nullptr || c == &target || !c->isAlive()) {
            continue;
        }
        if (c->distanceTo(static_cast<int>(tr), static_cast<int>(tc)) <= kSplashRadius) {
            c->takeDamage(splashDamage);
        }
    }
}

BurningDecorator::BurningDecorator(std::unique_ptr<Tower> wrapped)
    : TowerDecorator(std::move(wrapped)) {}

std::string BurningDecorator::effectTag() const { return "BURN"; }

void BurningDecorator::fireAt(Critter& target, std::vector<Critter*>& allCritters) {
    TowerDecorator::fireAt(target, allCritters);
    target.applyBurn(kBurnDps, kBurnDuration);
}

FreezingDecorator::FreezingDecorator(std::unique_ptr<Tower> wrapped)
    : TowerDecorator(std::move(wrapped)) {}

std::string FreezingDecorator::effectTag() const { return "FREEZE"; }

void FreezingDecorator::fireAt(Critter& target, std::vector<Critter*>& allCritters) {
    TowerDecorator::fireAt(target, allCritters);
    target.applySlow(kFreezeFactor, kFreezeDuration);
}
