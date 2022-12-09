#include "vector.h"

#include <glm/gtx/norm.hpp>
#include <cmath>

namespace Game {

    glm::vec2 normalizeOr(const glm::vec2 &vector, const glm::vec2 &alternative) {
        if(glm::length2(vector) > 1e-6f) {
            return glm::normalize(vector);
        } else {
            return alternative;
        }
    }

    glm::vec2 scaleTo(const glm::vec2 &vector, float targetLength) {
        return glm::normalize(vector) * targetLength;
    }

    glm::vec2 conj(const glm::vec2 &a) {
        return {a.x, -a.y};
    }

    glm::vec2 cmul(const glm::vec2 &a, const glm::vec2 &b) {
        return {a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x};
    }

    glm::vec2 cdiv(const glm::vec2 &a, const glm::vec2 &b) {
        return cmul(a, conj(b)) / glm::length2(b);
    }

    glm::vec2 polar(float angle, float length) {
        return {cos(angle)*length, sin(angle)*length};
    }

}