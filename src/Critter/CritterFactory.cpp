#include "CritterFactory.h"

Critter CritterFactory::createCritter(CritterKind kind, int waveNb) {	
	int maxHp = 100;
	float speed = 0.4f;
	
	switch (kind) {
	case CritterKind::Normal:
		maxHp = static_cast<int>(100.f * (1.f + (waveNb / 10.0f)));
		speed = 0.4f + (0.1f * waveNb);
		break;
	case CritterKind::Armored:
		maxHp = static_cast<int>(130.f * (1.f + (waveNb / 10.0f)));
		speed = 0.2f;
		break;
	case CritterKind::Fast:
		maxHp = static_cast<int>(80.f * (1.f + (waveNb / 10.0f)));
		speed = 3.5f;
		break;
	}

	return Critter(kind, maxHp, speed, 0, 0, waveNb);
}