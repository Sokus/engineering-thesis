#ifndef RNG_H
#define RNG_H

#include <raylib.h>

int RandomInt(int min, int max);

float RandomFloat01();
float RandomFloat(float min, float max);

Vector2 RandomVector2(float minLength, float maxLength);

#endif