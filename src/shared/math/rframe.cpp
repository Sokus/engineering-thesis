#include "rframe.h"

namespace Game {

    void ReferenceFrame::Update(float dt) {
        position += velocity * dt;
        rotation += angularVelocity * dt;
    }
    
}