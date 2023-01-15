#include "drawqueue.h"

namespace Game {

    void DrawQueue::DrawLight(const Light &light) {
        lights.push_back(light);
    }

    void DrawQueue::Clear() {
        lights.clear();
    }

}