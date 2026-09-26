/**
 * @file TowerView.cpp
 * @brief Distinct silhouettes per tower type + selection/range feedback.
 */
#include "TowerView.h"

namespace {

sf::Vector2f cellCenter(const Tower& t, float cellSize, float ox, float oy) {
    return {ox + t.getCol() * cellSize + cellSize * 0.5f,
            oy + t.getRow() * cellSize + cellSize * 0.5f};
}

} // namespace

void TowerView::draw(sf::RenderWindow& window,
                     const std::vector<std::unique_ptr<Tower>>& towers,
                     float cellSize,
                     float originX,
                     float originY,
                     int selected) const {
    for (std::size_t i = 0; i < towers.size(); ++i) {
        drawOne(window, *towers[i], cellSize, originX, originY,
                static_cast<int>(i) == selected);
    }
}

void TowerView::drawOne(sf::RenderWindow& window, const Tower& tower,
                        float cellSize, float originX, float originY,
                        bool selected) const {
    const sf::Vector2f c = cellCenter(tower, cellSize, originX, originY);
    const float s = cellSize;

    if (selected) {
        sf::CircleShape range(tower.getRange() * s);
        range.setOrigin(range.getRadius(), range.getRadius());
        range.setPosition(c);
        range.setFillColor(sf::Color(255, 255, 255, 28));
        range.setOutlineThickness(1.5f);
        range.setOutlineColor(sf::Color(255, 255, 255, 140));
        window.draw(range);
    }

    sf::CircleShape pad(s * 0.38f);
    pad.setOrigin(pad.getRadius(), pad.getRadius());
    pad.setPosition(c);
    pad.setFillColor(sf::Color(45, 48, 55));
    pad.setOutlineThickness(selected ? 2.5f : 1.f);
    pad.setOutlineColor(selected ? sf::Color(255, 220, 80) : sf::Color(20, 22, 28));
    window.draw(pad);

    const std::string type = tower.getTypeName();
    if (type == "Direct") {
        sf::CircleShape body(s * 0.22f);
        body.setOrigin(body.getRadius(), body.getRadius());
        body.setPosition(c);
        body.setFillColor(sf::Color(55, 58, 68));
        body.setOutlineThickness(1.5f);
        body.setOutlineColor(sf::Color(180, 185, 200));
        window.draw(body);

        sf::RectangleShape barrel(sf::Vector2f(s * 0.42f, s * 0.12f));
        barrel.setOrigin(0.f, barrel.getSize().y * 0.5f);
        barrel.setPosition(c);
        barrel.setFillColor(sf::Color(30, 32, 40));
        barrel.setOutlineColor(sf::Color(140, 145, 160));
        barrel.setOutlineThickness(1.f);
        window.draw(barrel);

        sf::CircleShape muzzle(s * 0.07f);
        muzzle.setOrigin(muzzle.getRadius(), muzzle.getRadius());
        muzzle.setPosition(c.x + s * 0.42f, c.y);
        muzzle.setFillColor(sf::Color(200, 60, 50));
        window.draw(muzzle);
    } else if (type == "Area") {
        sf::CircleShape outer(s * 0.26f);
        outer.setOrigin(outer.getRadius(), outer.getRadius());
        outer.setPosition(c);
        outer.setFillColor(sf::Color(190, 95, 25));
        outer.setOutlineThickness(2.f);
        outer.setOutlineColor(sf::Color(255, 180, 70));
        window.draw(outer);

        sf::CircleShape inner(s * 0.12f);
        inner.setOrigin(inner.getRadius(), inner.getRadius());
        inner.setPosition(c);
        inner.setFillColor(sf::Color(40, 25, 15));
        window.draw(inner);

        for (int i = 0; i < 4; ++i) {
            const float ang = i * 3.14159265f * 0.5f + 0.4f;
            sf::RectangleShape tick(sf::Vector2f(s * 0.18f, s * 0.05f));
            tick.setOrigin(0.f, tick.getSize().y * 0.5f);
            tick.setPosition(c);
            tick.setRotation(ang * 180.f / 3.14159265f);
            tick.setFillColor(sf::Color(255, 210, 90));
            window.draw(tick);
        }
    } else {
        sf::ConvexShape gem;
        gem.setPointCount(4);
        gem.setPoint(0, {0.f, -s * 0.32f});
        gem.setPoint(1, {s * 0.22f, 0.f});
        gem.setPoint(2, {0.f, s * 0.32f});
        gem.setPoint(3, {-s * 0.22f, 0.f});
        gem.setPosition(c);
        gem.setFillColor(sf::Color(90, 190, 240));
        gem.setOutlineThickness(2.f);
        gem.setOutlineColor(sf::Color(220, 245, 255));
        window.draw(gem);

        sf::ConvexShape core;
        core.setPointCount(4);
        core.setPoint(0, {0.f, -s * 0.14f});
        core.setPoint(1, {s * 0.09f, 0.f});
        core.setPoint(2, {0.f, s * 0.14f});
        core.setPoint(3, {-s * 0.09f, 0.f});
        core.setPosition(c);
        core.setFillColor(sf::Color(200, 240, 255, 200));
        window.draw(core);
    }

    for (int i = 0; i < tower.getLevel(); ++i) {
        sf::CircleShape gem(s * 0.06f);
        gem.setOrigin(gem.getRadius(), gem.getRadius());
        const float x = c.x - s * 0.18f + i * s * 0.18f;
        gem.setPosition(x, c.y + s * 0.28f);
        gem.setFillColor(sf::Color(255, 220, 60));
        gem.setOutlineThickness(1.f);
        gem.setOutlineColor(sf::Color(120, 90, 20));
        window.draw(gem);
    }
}
