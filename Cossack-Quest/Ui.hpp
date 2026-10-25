#pragma once

#include <raylib.h>

// 2D overlay canvas that scales with the window. Between begin() and end() everything is drawn in
// reference pixels: the canvas is always REF_HEIGHT units tall and as wide as the window's aspect
// ratio allows, so textures and text keep the same proportions at any window size.
class Ui
{
public:
    static constexpr float REF_HEIGHT = 720.0f;

    void begin();
    void end() const;

    float width() const { return size.x; }
    float height() const { return size.y; }

    // Point on the canvas from normalized coordinates: {0, 0} is top-left, {1, 1} is bottom-right
    Vector2 anchor(Vector2 normalized) const;

    // Draw the src region of a texture so that its pivot (normalized, {0.5, 1} is bottom center) lands on position
    static void drawTexture(Texture2D texture, Rectangle src, Vector2 position, Vector2 pivot, float scale = 1.0f,
                            Color tint = WHITE);
    static void drawTexture(Texture2D texture, Vector2 position, Vector2 pivot, float scale = 1.0f,
                            Color tint = WHITE);
    // Draw text horizontally centered on position.x, with its top at position.y
    static void drawTextCentered(const char* text, Vector2 position, float fontSize, Color color);

private:
    Camera2D camera{};
    Vector2 size{};
};
