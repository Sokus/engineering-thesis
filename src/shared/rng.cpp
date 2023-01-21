#include "rng.h"

#include <random>

std::mt19937 rng;

int RandomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

float RandomFloat01() {
    std::uniform_real_distribution<float> dist(0, 1);
    return dist(rng);
}

float RandomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

Vector2 RandomVector2(float minLength, float maxLength) {
    float length = RandomFloat(minLength, maxLength);
    float arg = RandomFloat(0, 2*PI);
    return {cosf(arg)*length, sinf(arg)*length};
}