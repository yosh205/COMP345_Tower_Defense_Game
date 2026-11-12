/**
 * @file CritterFactory.cpp
 * @brief Implements CritterFactory. Function docs are in CritterFactory.h.
 */
#include "CritterFactory.h"

#include <algorithm>

Critter CritterFactory::createCritter(CritterKind kind, int waveNb) {
    waveNb = std::max(1, waveNb);
    // Base values at wave 0; hit points grow by 10% and speed by 0.1 cells/s per wave.
    int baseHp = 100;
    float baseSpeed = 0.4f;
    switch (kind) {
        case CritterKind::Normal:
            baseHp = 100;
            baseSpeed = 0.4f;
            break;
        case CritterKind::Fast:     // fewer hit points, faster
            baseHp = 80;
            baseSpeed = 0.8f;
            break;
        case CritterKind::Armored:  // more hit points, slower
            baseHp = 130;
            baseSpeed = 0.2f;
            break;
    }
    const int maxHp = static_cast<int>(static_cast<float>(baseHp) * (1.f + waveNb / 10.f));
    const float speed = baseSpeed + 0.1f * static_cast<float>(waveNb);
    return Critter(kind, maxHp, speed, 0, 0, waveNb);
}

WavePlan CritterFactory::planWave(int waveNb) {
    waveNb = std::max(1, waveNb);
    WavePlan plan;
    plan.normal = std::min(8 + waveNb, 20);                                // 9, 10, 11, ...
    plan.fast = waveNb >= 2 ? std::min(2 * (waveNb - 1), 12) : 0;         // from wave 2
    plan.armored = waveNb >= 3 ? std::min(2 * (waveNb - 2), 12) : 0;      // from wave 3
    return plan;
}

std::vector<Critter> CritterFactory::createWave(int waveNb) {
    const WavePlan plan = planWave(waveNb);
    std::vector<Critter> wave;
    wave.reserve(static_cast<std::size_t>(plan.total()));
    // Fastest first, so nobody has to wait behind a slower critter.
    for (int i = 0; i < plan.fast; ++i) {
        wave.push_back(createCritter(CritterKind::Fast, waveNb));
    }
    for (int i = 0; i < plan.normal; ++i) {
        wave.push_back(createCritter(CritterKind::Normal, waveNb));
    }
    for (int i = 0; i < plan.armored; ++i) {
        wave.push_back(createCritter(CritterKind::Armored, waveNb));
    }
    return wave;
}
