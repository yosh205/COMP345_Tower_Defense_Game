/**
* @file CritterView.cpp
* @brief Distinct silhouettes per critter type
*/
#include "CritterView.h"
#include <algorithm>

namespace {
	
sf::Vector2f cellCenter(const Critter& c, float cellSize, float ox, float oy) {
	return { ox + c.getCol() * cellSize + cellSize * 0.5f,
            oy + c.getRow() * cellSize + cellSize * 0.5f };
}

}

sf::Color CritterView::getColorForKind(CritterKind kind) const {
	switch (kind) {
		case CritterKind::Normal:	return sf::Color(220, 80, 80);	//Red
		case CritterKind::Armored:	return sf::Color(70, 130, 220);	//Blue
    case CritterKind::Fast:		return sf::Color(245, 195, 65);	//Yellow
		default:					return sf::Color::White;
	}
}

void CritterView::draw(sf::RenderWindow& window,
						const std::vector<Critter>& critters,
						float cellSize,
						float originX,
						float originY) const {
    for (const auto& critter : critters) {
		drawOne(window, critter, cellSize, originX, originY);
	}
}

void CritterView::drawOne(sf::RenderWindow& window, const Critter& critter,
							float cellSize, float originX, float originY) const {
	if (!critter.isAlive()) {
		return;
	}

	const sf::Vector2f c = cellCenter(critter, cellSize, originX, originY);
	const float s = cellSize;

	// 1. Render Critter Base Silhouette by Kind
	const CritterKind kind = critter.getKind();

    if (kind == CritterKind::Normal) {
        // Round Body
        sf::CircleShape body(s * 0.26f);
        body.setOrigin(body.getRadius(), body.getRadius());
        body.setPosition(c);
        body.setFillColor(getColorForKind(kind));
        body.setOutlineThickness(1.5f);
        body.setOutlineColor(sf::Color(140, 30, 30));
        window.draw(body);

        // Inner Core Dot
        sf::CircleShape core(s * 0.09f);
        core.setOrigin(core.getRadius(), core.getRadius());
        core.setPosition(c);
        core.setFillColor(sf::Color(255, 180, 180));
        window.draw(core);

    }
    else if (kind == CritterKind::Armored) {
        // Heavy Octagon / Square Shield Base
        sf::RectangleShape shield(sf::Vector2f(s * 0.46f, s * 0.46f));
        shield.setOrigin(shield.getSize().x * 0.5f, shield.getSize().y * 0.5f);
        shield.setPosition(c);
        shield.setFillColor(getColorForKind(kind));
        shield.setOutlineThickness(2.f);
        shield.setOutlineColor(sf::Color(200, 220, 255));
        window.draw(shield);

        // Inner Armor Plate
        sf::RectangleShape plate(sf::Vector2f(s * 0.24f, s * 0.24f));
        plate.setOrigin(plate.getSize().x * 0.5f, plate.getSize().y * 0.5f);
        plate.setPosition(c);
        plate.setFillColor(sf::Color(30, 50, 90));
        window.draw(plate);

    }
    else { // Fast
        // Forward-facing Triangle / Diamond Silhouette
        sf::ConvexShape diamond;
        diamond.setPointCount(4);
        diamond.setPoint(0, { 0.f, -s * 0.28f });
        diamond.setPoint(1, { s * 0.20f, 0.f });
        diamond.setPoint(2, { 0.f, s * 0.28f });
        diamond.setPoint(3, { -s * 0.20f, 0.f });
        diamond.setPosition(c);
        diamond.setFillColor(getColorForKind(kind));
        diamond.setOutlineThickness(1.5f);
        diamond.setOutlineColor(sf::Color(255, 240, 150));
        window.draw(diamond);

        // Speed Streak Core
        sf::ConvexShape streak;
        streak.setPointCount(4);
        streak.setPoint(0, { 0.f, -s * 0.14f });
        streak.setPoint(1, { s * 0.08f, 0.f });
        streak.setPoint(2, { 0.f, s * 0.14f });
        streak.setPoint(3, { -s * 0.08f, 0.f });
        streak.setPosition(c);
        streak.setFillColor(sf::Color(255, 255, 220));
        window.draw(streak);
    }

    // 2. Health Bar Background
    const float barW = s * 0.65f;
    const float barH = s * 0.08f;
    const float barX = c.x - barW * 0.5f;
    const float barY = c.y - s * 0.36f;

    sf::RectangleShape hpBg(sf::Vector2f(barW, barH));
    hpBg.setPosition(barX, barY);
    hpBg.setFillColor(sf::Color(40, 15, 15));
    hpBg.setOutlineThickness(1.f);
    hpBg.setOutlineColor(sf::Color(15, 15, 20));
    window.draw(hpBg);

    // 3. Health Bar Fill
    float ratio = static_cast<float>(critter.getHitPoints()) /
        static_cast<float>(critter.getMaxHitPoints());
    ratio = std::clamp(ratio, 0.f, 1.f);

    sf::RectangleShape hpFill(sf::Vector2f(barW * ratio, barH));
    hpFill.setPosition(barX, barY);
    hpFill.setFillColor((ratio > 0.4f) ? sf::Color(60, 220, 80) : sf::Color(230, 60, 50));
    window.draw(hpFill);
}