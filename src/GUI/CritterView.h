/**
* @file CritterView.h
* @brief Draws Critters 
*/

#ifndef CritterView_h
#define CritterView_h

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "../Critter/Critter.h"

class CritterView {
public:
	void draw(sf::RenderWindow& window,
		const std::vector<Critter>& critters,
		float cellSize,
		float originX,
		float originY) const;
private:
	void drawOne(sf::RenderWindow& window, const Critter& critter,
				float cellSize, float originX, float originY) const;
	sf::Color getColorForKind(CritterKind kind) const;
};

#endif // CritterView_h