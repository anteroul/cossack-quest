#include "Ui.hpp"

void Ui::begin()
{
    camera.zoom = GetScreenHeight() / REF_HEIGHT;
    size = {GetScreenWidth() / camera.zoom, REF_HEIGHT};
    BeginMode2D(camera);
}

void Ui::end() const
{
    EndMode2D();
}

Vector2 Ui::anchor(Vector2 normalized) const
{
    return {size.x * normalized.x, size.y * normalized.y};
}

void Ui::drawTexture(Texture2D texture, Rectangle src, Vector2 position, Vector2 pivot, float scale, Color tint)
{
    const float w = src.width * scale;
    const float h = src.height * scale;
    DrawTexturePro(texture, src, {position.x - w * pivot.x, position.y - h * pivot.y, w, h}, {0, 0}, 0.0f, tint);
}

void Ui::drawTexture(Texture2D texture, Vector2 position, Vector2 pivot, float scale, Color tint)
{
    drawTexture(texture, {0, 0, (float)texture.width, (float)texture.height}, position, pivot, scale, tint);
}

void Ui::drawTextCentered(const char* text, Vector2 position, float fontSize, Color color)
{
    const int size = static_cast<int>(fontSize);
    DrawText(text, static_cast<int>(position.x) - MeasureText(text, size) / 2, static_cast<int>(position.y), size,
             color);
}
