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
	 * @param selectedId Id of the critter being inspected (drawn with a ring), or -1 for none.
	 */
	void draw(sf::RenderWindow& window,
		const std::vector<Critter>& critters,
		float cellSize,
		float originX,
		float originY, int selectedId = -1) const;
	/**
	 * @brief Finds the critter under a mouse click.
	 *
	 * Critters move continuously between cells, so the click is tested against
	 * each critter's drawn position (distance to its centre), not against a grid
	 * cell. If several critters overlap the click, the closest one wins.
	 * @param critters The critters on the map.
	 * @param mouse    Click position, in window pixels.
	 * @param cellSize Size of one map cell, in pixels.
	 * @param originX  Left edge of the map, in pixels.
	 * @param originY  Top edge of the map, in pixels.
	 * @return Id (Critter::getId()) of the clicked living critter, or -1 if none.
	 */
	static int pickCritterId(const std::vector<Critter>& critters,
		sf::Vector2f mouse,
		float cellSize,
		float originX,
		float originY);

	/**
	 * @brief Looks a critter up by id.
	 *
	 * Use this every frame instead of keeping an index or pointer: the generator
	 * removes critters and moves the rest in memory, which would make those wrong.
	 * @param critters The critters on the map.
	 * @param id       Id from pickCritterId(); -1 means none.
	 * @return The living critter with that id, or nullptr if it died, escaped or never existed.
	 */
	static const Critter* findById(const std::vector<Critter>& critters, int id);

	/**
	 * @brief Draws a panel with a critter's characteristics: kind, level, hit points,
	 * speed (and whether it is slowed), strength, reward and coins stolen at the exit.
	 * @param window   The window to draw into.
	 * @param critter  The critter to describe.
	 * @param originX  Left edge of the panel, in pixels.
	 * @param originY  Top edge of the panel, in pixels.
	 * @param width    Panel width, in pixels.
	 * @param height   Panel height, in pixels.
	 */
	static void drawCharacteristics(sf::RenderWindow& window, const Critter& critter,
		float originX, float originY, float width, float height);

private:
	/**
	 * @brief Draws one critter (skipped if dead) and its health bar.
	 * @param window   The window to draw into.
	 * @param critter  The critter to draw.
	 * @param cellSize Size of one map cell, in pixels.
	 * @param originX  Left edge of the map, in pixels.
	 * @param originY  Top edge of the map, in pixels.
	 * @param selected True to draw a white selection ring around the critter.
	 */
	void drawOne(sf::RenderWindow& window, const Critter& critter,
				float cellSize, float originX, float originY, bool selected) const;
	/**
	 * @param kind A critter kind.
	 * @return The fill colour used for a critter kind.
	 */
	sf::Color getColorForKind(CritterKind kind) const;
};

#endif // CritterView_h