#ifndef MATH_GEOM_H
#define MATH_GEOM_H

#include <raylib.h>

Vector2 GetRectCenter(const Rectangle &rect);
Rectangle RectCentered(Vector2 center, Vector2 size);

#endif