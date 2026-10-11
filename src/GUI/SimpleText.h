/**
 * @file SimpleText.h
 * @brief Tiny 5x7 bitmap font (avoids SFML FreeType / MinGW link issues).
 */
#ifndef SimpleText_h
#define SimpleText_h

#include <SFML/Graphics.hpp>
#include <string>

/**
 * @brief Draws text with a built-in 5x7 pixel font, without any font file.
 *
 * Each character is a 5x7 grid of small rectangles. Used by Hud for all menu
 * and sidebar text, so the game does not depend on SFML's FreeType font loading.
 */
class SimpleText {
public:
    /**
     * @brief Draws uppercase-friendly text using filled pixel rectangles.
     * @param window The window to draw into.
     * @param text   The text to draw (lowercase letters are drawn as uppercase).
     * @param x      Left edge of the first character, in pixels.
     * @param y      Top edge of the text, in pixels.
     * @param scale  Pixel size of one font pixel (2–3 is readable).
     * @param color  Colour of the text.
     */
    static void draw(sf::RenderWindow& window, const std::string& text,
                     float x, float y, float scale, sf::Color color);
};

#endif // SimpleText_h
