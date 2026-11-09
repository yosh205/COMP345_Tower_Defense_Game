#include <string>
#include <memory>
#include "Critter.h"

class CritterFactory {
	public: 
		static Critter createCritter(CritterKind kind, int waveNb);
};