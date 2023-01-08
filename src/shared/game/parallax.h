#ifndef PARALLAX_H
#define PARALLAX_H

#include "raylib.h"

#include <vector>

struct ParallaxLayer
{
    Texture2D texture;
    Vector3 position;
    Vector2 dimensions;

    /// How fast the layer scrolls horizontally even when the camera is not moving
    float natural_scroll_speed = 0;
    bool repeat_horizontally = true;
    float horizontal_offset = 0;

    void Update(float dt);
    void Draw(Vector2 camera_position) const;
};

struct ParallaxBackground
{
    std::vector<ParallaxLayer> parallax_layers;

    void AddParallaxLayer(const ParallaxLayer &layer);
    void Sort();
    void Update(float dt);
    void Draw(Vector2 camera_position) const;
    void Clear();
};

#endif // PARALLAX_H