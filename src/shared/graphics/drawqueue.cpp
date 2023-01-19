#include "drawqueue.h"

#include <system/pi_time.h>

namespace Game {

    void DrawQueue::DrawLight(const Light &light) {
        lights.push_back(light);
    }

    void DrawQueue::DrawEnergySphere(const Light &sphere) {
        energySpheres.push_back(sphere);
    }

    void DrawQueue::DrawShockwaveEffect(const ShockwaveEffect &effect) {
        shockwaves.push_back(effect);
    }

    void DrawQueue::Clear() {
        lights.clear();
        energySpheres.clear();
        shockwaves.clear();
        bloodEffectStrength = grayscaleEffectStrength = blurStrength = 0;
    }

}