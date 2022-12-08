#include "level.h"

#include "macros.h"
#include <algorithm>
#include <cmath>

namespace Game {

    float floorMod(float a, float b) {
        return fmod(fmod(a,b)+b, b);
    }

    bool intersects(const Rectangle &a, const Rectangle &b) {
        return MAX(a.x, b.x) <= MIN(a.x+a.width,  b.x+b.width) &&
               MAX(a.y, b.y) <= MIN(a.y+a.height, b.y+b.height); 
    }

    void ParallaxLayer::Update(float dt) {
        horizontalOffset += naturalScrollSpeed * dt;
    }

    void ParallaxLayer::Draw(glm::vec2 cameraPosition) const {

        auto displacementFromCamera = glm::vec2(position) - cameraPosition;
        displacementFromCamera.x += horizontalOffset;

        Rectangle source, dest, visibleArea;

        source.x = source.y = 0;
        source.width = texture.width;
        source.height = texture.height;

        dest.x = displacementFromCamera.x / position.z;
        dest.y = displacementFromCamera.y / position.z;
        dest.width = dimensions.x;
        dest.height = dimensions.y;

        visibleArea.x = visibleArea.y = 0;
        visibleArea.width = GetScreenWidth();
        visibleArea.height = GetScreenHeight();
        
        if(repeatHorizontally) {

            // Position the layer to the left of the visible area
            dest.x = floorMod(dest.x-visibleArea.x, visibleArea.width) + visibleArea.x - visibleArea.width;

            // Draw multiple times side by side until we fill the entire width of the screen
            do {
                DrawTexturePro(texture, source, dest, {0,0}, 0, WHITE);
                dest.x += dest.width;
            } while(intersects(dest, visibleArea));

        } else if(intersects(dest, visibleArea)) {

            DrawTexturePro(texture, source, dest, {0,0}, 0, WHITE);

        } 
    }

    void Level::AddParallaxLayer(const ParallaxLayer &layer) {

        ASSERT(layer.position.z > 1);
        parallaxLayers.push_back(layer);

        // keep layers sorted by position.z descending to ensure proper draw order
        std::sort(
            parallaxLayers.begin(), parallaxLayers.end(), 
            [](const ParallaxLayer &a, const ParallaxLayer &b){return a.position.z > b.position.z;}
        );
    }

    void Level::Update(float dt) {
        for(auto &layer : parallaxLayers) {
            layer.Update(dt);
        }
    }

    void Level::DrawBackground(glm::vec2 cameraPosition) const {
        for(auto &layer : parallaxLayers) {
            layer.Draw(cameraPosition);
        }
    }
}