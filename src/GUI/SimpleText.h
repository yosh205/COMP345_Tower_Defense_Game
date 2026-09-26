/**
 * @file SimpleText.h
 * @brief Tiny 5x7 bitmap font (avoids SFML FreeType / MinGW link issues).
 */
#ifndef SimpleText_h
#define SimpleText_h

#include <SFML/Graphics.hpp>
#include <string>

class SimpleText {
public:
    /**
     * @brief Draws uppercase-friendly text using filled pixel rectangles.
     * @param scale Pixel size of one font pixel (2–3 is readable).
     */
    static void draw(sf::RenderWindow& window, const std::string& text,
                     float x, float y, float scale, sf::Color color);
};

#endif // SimpleText_h
