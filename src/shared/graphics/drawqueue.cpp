#include "drawqueue.h"

namespace Game {

    void DrawQueue::DrawLight(const Light &light) {
        lights.push_back(light);
    }

    void DrawQueue::DrawEnergySphere(const Light &sphere) {
        energySpheres.push_back(sphere);
    }

    void DrawQueue::Clear() {
        lights.clear();
        energySpheres.clear();
    }

}