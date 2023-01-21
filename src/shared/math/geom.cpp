#include <math/geom.h>

Vector2 GetRectCenter(const Rectangle &rect) {
    return {rect.x+rect.width/2, rect.y+rect.height/2};
}
Rectangle RectCentered(Vector2 center, Vector2 size) {
    return {
        .x = center.x - size.x/2,
        .y = center.y - size.y/2,
        .width = size.x,
        .height = size.y 
    };
}