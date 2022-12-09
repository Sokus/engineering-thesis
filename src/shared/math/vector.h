#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

#include <glm/vec2.hpp>

namespace Game {

    /** Attempts to normalize the vector and return the result.
     * 
     *  Returns alternative if vector can not be normalized (because length(vector)==0).
     */
    glm::vec2 normalizeOr(const glm::vec2 &vector, const glm::vec2 &alternative);

    /// Computes a new vector with the same direction as input vector, but different length.
    glm::vec2 scaleTo(const glm::vec2 &vector, float targetLength);

    /// Complex conjugate of a vector.
    glm::vec2 conj(const glm::vec2 &a);

    /// Complex multiplication of 2 vectors.
    glm::vec2 cmul(const glm::vec2 &a, const glm::vec2 &b);

    /// Complex division of 2 vectors.
    glm::vec2 cdiv(const glm::vec2 &a, const glm::vec2 &b);

    /// Creates a 2D vector from polar coordinates.
    glm::vec2 polar(float angle, float length = 1);

}

#endif