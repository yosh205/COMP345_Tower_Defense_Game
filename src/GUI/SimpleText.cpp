/**
 * @file SimpleText.cpp
 * @brief 5x7 glyphs for A–Z, 0–9, and a few symbols used by the HUD.
 */
#include "SimpleText.h"

#include <cctype>
#include <cstdint>

namespace {

// Each glyph: 5 columns × 7 rows, bit0 = top row of column… packed as 7 bits per column.
// Layout: rows top→bottom in bits 0→6 of each of 5 column bytes.
using Glyph = std::uint8_t[5];

const Glyph* glyphFor(char ch) {
    static const Glyph G_SP = {0, 0, 0, 0, 0};
    static const Glyph G_0 = {0x3E, 0x51, 0x49, 0x45, 0x3E};
    static const Glyph G_1 = {0x00, 0x42, 0x7F, 0x40, 0x00};
    static const Glyph G_2 = {0x62, 0x51, 0x49, 0x49, 0x46};
    static const Glyph G_3 = {0x22, 0x41, 0x49, 0x49, 0x36};
    static const Glyph G_4 = {0x18, 0x14, 0x12, 0x7F, 0x10};
    static const Glyph G_5 = {0x27, 0x45, 0x45, 0x45, 0x39};
    static const Glyph G_6 = {0x3E, 0x49, 0x49, 0x49, 0x32};
    static const Glyph G_7 = {0x01, 0x71, 0x09, 0x05, 0x03};
    static const Glyph G_8 = {0x36, 0x49, 0x49, 0x49, 0x36};
    static const Glyph G_9 = {0x26, 0x49, 0x49, 0x49, 0x3E};
    static const Glyph G_A = {0x7E, 0x11, 0x11, 0x11, 0x7E};
    static const Glyph G_B = {0x7F, 0x49, 0x49, 0x49, 0x36};
    static const Glyph G_C = {0x3E, 0x41, 0x41, 0x41, 0x22};
    static const Glyph G_D = {0x7F, 0x41, 0x41, 0x22, 0x1C};
    static const Glyph G_E = {0x7F, 0x49, 0x49, 0x49, 0x41};
    static const Glyph G_F = {0x7F, 0x09, 0x09, 0x09, 0x01};
    static const Glyph G_G = {0x3E, 0x41, 0x49, 0x49, 0x3A};
    static const Glyph G_H = {0x7F, 0x08, 0x08, 0x08, 0x7F};
    static const Glyph G_I = {0x00, 0x41, 0x7F, 0x41, 0x00};
    static const Glyph G_J = {0x20, 0x40, 0x41, 0x3F, 0x01};
    static const Glyph G_K = {0x7F, 0x08, 0x14, 0x22, 0x41};
    static const Glyph G_L = {0x7F, 0x40, 0x40, 0x40, 0x40};
    static const Glyph G_M = {0x7F, 0x02, 0x0C, 0x02, 0x7F};
    static const Glyph G_N = {0x7F, 0x04, 0x08, 0x10, 0x7F};
    static const Glyph G_O = {0x3E, 0x41, 0x41, 0x41, 0x3E};
    static const Glyph G_P = {0x7F, 0x09, 0x09, 0x09, 0x06};
    static const Glyph G_Q = {0x3E, 0x41, 0x51, 0x21, 0x5E};
    static const Glyph G_R = {0x7F, 0x09, 0x19, 0x29, 0x46};
    static const Glyph G_S = {0x26, 0x49, 0x49, 0x49, 0x32};
    static const Glyph G_T = {0x01, 0x01, 0x7F, 0x01, 0x01};
    static const Glyph G_U = {0x3F, 0x40, 0x40, 0x40, 0x3F};
    static const Glyph G_V = {0x1F, 0x20, 0x40, 0x20, 0x1F};
    static const Glyph G_W = {0x7F, 0x20, 0x18, 0x20, 0x7F};
    static const Glyph G_X = {0x63, 0x14, 0x08, 0x14, 0x63};
    static const Glyph G_Y = {0x07, 0x08, 0x70, 0x08, 0x07};
    static const Glyph G_Z = {0x61, 0x51, 0x49, 0x45, 0x43};
    static const Glyph G_DOLLAR = {0x24, 0x2A, 0x7F, 0x2A, 0x12};
    static const Glyph G_SLASH = {0x20, 0x10, 0x08, 0x04, 0x02};
    static const Glyph G_PLUS = {0x08, 0x08, 0x3E, 0x08, 0x08};
    static const Glyph G_MINUS = {0x08, 0x08, 0x08, 0x08, 0x08};
    static const Glyph G_DOT = {0x00, 0x60, 0x60, 0x00, 0x00};
    static const Glyph G_COLON = {0x00, 0x36, 0x36, 0x00, 0x00};
    static const Glyph G_LP = {0x00, 0x1C, 0x22, 0x41, 0x00};
    static const Glyph G_RP = {0x00, 0x41, 0x22, 0x1C, 0x00};
    static const Glyph G_PCT = {0x23, 0x13, 0x08, 0x64, 0x62};

    ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    if (ch == ' ') return &G_SP;
    if (ch >= '0' && ch <= '9') {
        static const Glyph* digits[] = {
            &G_0, &G_1, &G_2, &G_3, &G_4, &G_5, &G_6, &G_7, &G_8, &G_9};
        return digits[ch - '0'];
    }
    if (ch >= 'A' && ch <= 'Z') {
        static const Glyph* letters[] = {
            &G_A, &G_B, &G_C, &G_D, &G_E, &G_F, &G_G, &G_H, &G_I, &G_J,
            &G_K, &G_L, &G_M, &G_N, &G_O, &G_P, &G_Q, &G_R, &G_S, &G_T,
            &G_U, &G_V, &G_W, &G_X, &G_Y, &G_Z};
        return letters[ch - 'A'];
    }
    switch (ch) {
        case '$': return &G_DOLLAR;
        case '/': return &G_SLASH;
        case '+': return &G_PLUS;
        case '-': return &G_MINUS;
        case '.': return &G_DOT;
        case ':': return &G_COLON;
        case '(': return &G_LP;
        case ')': return &G_RP;
        case '%': return &G_PCT;
        default: return &G_SP;
    }
}

} // namespace

void SimpleText::draw(sf::RenderWindow& window, const std::string& text,
                      float x, float y, float scale, sf::Color color) {
    sf::RectangleShape px(sf::Vector2f(scale, scale));
    px.setFillColor(color);
    float cx = x;
    for (char ch : text) {
        const Glyph* g = glyphFor(ch);
        for (int col = 0; col < 5; ++col) {
            const std::uint8_t bits = (*g)[col];
            for (int row = 0; row < 7; ++row) {
                if (bits & (1u << row)) {
                    px.setPosition(cx + col * scale, y + row * scale);
                    window.draw(px);
                }
            }
        }
        cx += 6.f * scale; // 5 px glyph + 1 px gap
    }
}
