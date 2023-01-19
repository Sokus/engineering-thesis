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
    Color text_dark;
    Color text_light;
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
    Base *active_element;

    Base spaces[max_element_count];
    int spaces_count = 0;
};

void Init();
void SetDefaultStyle(Style *style);
Style *GetCurrentStyle();
void SetPosition(float x, float y);
void SetOrigin(float x, float y);
void Begin();
void Add(Base *base);
void AddSpace();
void EndRow();
void End();

class Button
{
private:
    const char *label;

public:
    Base base;
    Vector2 label_size;

    Button(const char *label);
    bool IsHovered();
    bool IsPressed();
    bool IsReleased();
    void Draw();
};

class TextField
{
private:
    char *buffer;
    int buffer_capacity;

public:
    Base base;
    Vector2 text_size;

    TextField(char *buffer, int buffer_capacity);
    TextField(int max_character_count, const char *default_value);
    bool IsHovered();
    void Update();
    void Draw();
};

} // namespace UI

#endif // USER_INTERFACE