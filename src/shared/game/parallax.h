#ifndef PARALLAX_H
#define PARALLAX_H

#include "raylib.h"

#include <vector>

struct ParallaxLayer
{
    private:
    float offsetX = 0;

    public:

    Texture2D texture;
    Rectangle bounds;
    float z;
    float emissiveness = 0;

    /// How fast the layer scrolls horizontally even when the camera is not moving
    float scrollSpeed = 0;
    bool repeatX = true;

    void Update(float dt);
    void Draw(const Rectangle visibleArea) const;
};

struct ParallaxBackground
{
    std::vector<ParallaxLayer> parallax_layers;

    void AddParallaxLayer(const ParallaxLayer &layer);
    void Sort();
    void Update(float dt);
    void Draw(const Rectangle visibleArea) const;
    void Clear();
};

#endif // PARALLAX_H