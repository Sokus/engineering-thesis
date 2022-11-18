#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "raylib.h"

namespace UI {

struct Style
{
    Color fill_default;
    Color fill_active;
    Color text_default;
    Color text_suggestion;
    Color outline_active;
    Vector2 padding;
    Vector2 spacing;
};

struct Base
{
    Style *style;
    Vector2 min_size;
    Rectangle rect;
};

class Button
{
private:
    const char *label;
    Font *font;
    float font_size;

public:
    Base base;
    Vector2 label_size;

    Button(Style *style, Font *font, char *label, float font_size);
    bool IsHovered();
    bool IsPressed();
    bool IsReleased();
    void Draw();
};

class TextField
{
private:
    const Font *font;
    const char *label;
    float font_size;
    const char *default_value;
    static const int data_capacity = 32;
    char data[data_capacity + 1];
    int character_count;
    int max_character_count;
    bool active;

public:
    Base base;
    Vector2 text_size;

    TextField(Style *style, Font *font,
              int max_character_count, char *default_value,
              float font_size);
    bool IsHovered();
    void Update();
    void Draw();
};

} // namespace UI

#endif // USER_INTERFACE