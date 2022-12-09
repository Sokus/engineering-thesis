#ifndef MATH_RFRAME_H
#define MATH_RFRAME_H

#include <glm/vec2.hpp>

namespace Game {

    struct ReferenceFrame
    {
        glm::vec2 position = {0,0};
        glm::vec2 velocity = {0,0};
        float rotation = 0;
        float angularVelocity = 0;

        void Update(float dt);
    };

}
#endif