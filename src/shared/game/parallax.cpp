#include "parallax.h"

#include "raylib.h"
#include "raymath.h"

#include "macros.h"

#include <algorithm>

Vector2 GetRectCenter(const Rectangle &rect) {
    return {rect.x+rect.width/2, rect.y+rect.height/2};
}
Rectangle RectCentered(Vector2 center, Vector2 size) {
    return {
        .x = center.x - size.x/2,
        .y = center.y - size.y/2,
        .width = size.x,
        .height = size.y 
    };
}

float FloorMod(float a, float b)
{
    return fmodf(fmodf(a, b) + b, b);
}

void ParallaxLayer::Update(float dt)
{
    offsetX += scrollSpeed * dt;
}

void ParallaxLayer::Draw(const Rectangle visibleArea) const {

    Vector2 cameraPos = GetRectCenter(visibleArea), 
            centerPos = GetRectCenter(bounds); 
    centerPos.x += offsetX;

    // Apply parallax effect
    Vector2 displacement = Vector2Subtract(centerPos, cameraPos);
    centerPos = Vector2Add(
        cameraPos,
        Vector2Divide(displacement, {z,z})
    );

    

    Rectangle source = {0, 0, (float)texture.width, (float)texture.height};
    Rectangle dest = RectCentered(centerPos, {bounds.width, bounds.height});

    if(repeatX) {

        // Position the layer to the left of the visible area
        dest.x = FloorMod(dest.x - visibleArea.x, visibleArea.width) + visibleArea.x - visibleArea.width;
        do {
            DrawTexturePro(texture, source, dest, {0, 0}, 0, WHITE);
            dest.x += dest.width;
        } while (CheckCollisionRecs(dest, visibleArea));

    } else if(CheckCollisionRecs(dest, visibleArea)) {
        DrawTexturePro(texture, source, dest, {0, 0}, 0, WHITE);
    }
}

void ParallaxBackground::AddParallaxLayer(const ParallaxLayer &layer)
{
    ASSERT(layer.z > 1);
    parallax_layers.push_back(layer);
}

void ParallaxBackground::Sort()
{
    // keep layers sorted by position.z descending to ensure proper draw order
    std::sort(
        parallax_layers.begin(), parallax_layers.end(),
        [](const ParallaxLayer &a, const ParallaxLayer &b)
        { return a.z > b.z; }
    );
}

void ParallaxBackground::Update(float dt)
{
    for (auto &layer : parallax_layers)
        layer.Update(dt);
}

void ParallaxBackground::Draw(const Rectangle visibleArea) const
{
    for (auto &layer : parallax_layers)
        layer.Draw(visibleArea);
}

void ParallaxBackground::Clear()
{
    for (auto &layer : parallax_layers)
        UnloadTexture(layer.texture);
}

