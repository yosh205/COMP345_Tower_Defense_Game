/**
 * @file CritterView.h
 * @brief Declares CritterView, which draws the critters on the map.
 */

#ifndef CritterView_h
#define CritterView_h

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "../Critter/Critter.h"

/**
 * @brief Draws each critter with a shape and colour that show its kind, plus a health bar.
 *
 * Normal = red circle, Armored = blue square, Fast = yellow diamond. The health bar
 * above each critter turns red below 40% hit points. CritterView only reads the
 * critters, so drawing never changes the game state.
 */
class CritterView {
public:
	/**
	 * @brief Draws every living critter in the list.
	 * @param window   The window to draw into.
	 * @param critters The critters on the map (CritterGroupGenerator::getActiveCritters()).
	 * @param cellSize Size of one map cell, in pixels.
	 * @param originX  Left edge of the map, in pixels.
	 * @param originY  Top edge of the map, in pixels.
	 */
	void draw(sf::RenderWindow& window,
		const std::vector<Critter>& critters,
		float cellSize,
		float originX,
		float originY) const;
private:
	/** @brief Draws one critter (skipped if dead) and its health bar. */
	void drawOne(sf::RenderWindow& window, const Critter& critter,
				float cellSize, float originX, float originY) const;
	/** @return The fill colour used for a critter kind. */
	sf::Color getColorForKind(CritterKind kind) const;
};

#endif // CritterView_h