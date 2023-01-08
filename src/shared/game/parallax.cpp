#include "parallax.h"

#include "raylib.h"
#include "raymath.h"

#include "macros.h"

#include <algorithm>

float FloorMod(float a, float b)
{
    return fmodf(fmodf(a, b) + b, b);
}

void ParallaxLayer::Update(float dt)
{
    horizontal_offset += natural_scroll_speed * dt;
}

void ParallaxLayer::Draw(Vector2 cameraPosition) const
{

    Vector2 parallax_2d_pos = Vector2{position.x, position.y};
    Vector2 displacementFromCamera = Vector2Subtract(parallax_2d_pos, cameraPosition);
    displacementFromCamera.x += horizontal_offset;

    Rectangle source, dest, visibleArea;

    source.x = source.y = 0;
    source.width = (float)texture.width;
    source.height = (float)texture.height;

    dest.x = displacementFromCamera.x / position.z;
    dest.y = displacementFromCamera.y / position.z;
    dest.width = dimensions.x;
    dest.height = dimensions.y;

    visibleArea.x = visibleArea.y = 0;
    visibleArea.width = (float)GetScreenWidth();
    visibleArea.height = (float)GetScreenHeight();

    if (repeat_horizontally)
    {

        // Position the layer to the left of the visible area
        dest.x = FloorMod(dest.x - visibleArea.x, visibleArea.width) + visibleArea.x - visibleArea.width;

        // Draw multiple times side by side until we fill the entire width of the screen
        do
        {
            DrawTexturePro(texture, source, dest, {0, 0}, 0, WHITE);
            dest.x += dest.width;
        } while (CheckCollisionRecs(dest, visibleArea));
    }
    else if (CheckCollisionRecs(dest, visibleArea))
    {
        DrawTexturePro(texture, source, dest, {0, 0}, 0, WHITE);
    }
}

void ParallaxBackground::AddParallaxLayer(const ParallaxLayer &layer)
{
    ASSERT(layer.position.z > 1);
    parallax_layers.push_back(layer);
}

void ParallaxBackground::Sort()
{
    // keep layers sorted by position.z descending to ensure proper draw order
    std::sort(
        parallax_layers.begin(), parallax_layers.end(),
        [](const ParallaxLayer &a, const ParallaxLayer &b)
        { return a.position.z > b.position.z; }
    );
}

void ParallaxBackground::Update(float dt)
{
    for (auto &layer : parallax_layers)
        layer.Update(dt);
}

void ParallaxBackground::Draw(Vector2 cameraPosition) const
{
    for (auto &layer : parallax_layers)
        layer.Draw(cameraPosition);
}

void ParallaxBackground::Clear()
{
    for (auto &layer : parallax_layers)
        UnloadTexture(layer.texture);
}

