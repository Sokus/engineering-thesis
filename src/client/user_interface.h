#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "raylib.h"

namespace UI {

struct Style
{
    Font font;
    float font_size;
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

struct LayoutRow
{
    int start;
    int count;
    Vector2 min_size;
};

struct Layout
{
    Style default_style;
    Style *current_style;

    Vector2 position;
    Vector2 origin;

    static const int max_element_count = 64;
    Base *elements[max_element_count];
    int element_count;
    static const int max_row_count = 32;
    LayoutRow rows[max_row_count];
    int row_count;
    Vector2 min_size;
    Vector2 element_offset;
};

void Init();
void SetDefaultStyle(Style *style);
Style *GetCurrentStyle();
void SetPosition(float x, float y);
void SetOrigin(float x, float y);
void Begin();
void Add(Base *base);
void EndRow();
void End();

class Button
{
private:
    const char *label;

public:
    Base base;
    Vector2 label_size;

    Button(char *label);
    bool IsHovered();
    bool IsPressed();
    bool IsReleased();
    void Draw();
};

class TextField
{
private:
    const char *label;
    const char *default_value;
    static const int data_capacity = 32;
    char data[data_capacity + 1];
    int character_count;
    int max_character_count;
    bool active;

public:
    Base base;
    Vector2 text_size;

    TextField(int max_character_count, char *default_value);
    bool IsHovered();
    void Update();
    void Draw();
};

} // namespace UI

#endif // USER_INTERFACE