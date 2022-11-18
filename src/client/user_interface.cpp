#include "user_interface.h"

#include <string.h>

namespace UI {

Button::Button(Style *style, Font *font, char *label, float font_size)
: font(font), label(label), font_size(font_size)
{
    label_size = MeasureTextEx(*font, label, font_size, 0.0f);
    base.min_size.x = label_size.x + 2.0f*style->padding.x;
    base.min_size.y = label_size.y + 2.0f*style->padding.y;
    base.style = style;
}

bool Button::IsHovered()
{
    return CheckCollisionPointRec(GetMousePosition(), base.rect);
}

bool Button::IsPressed()
{
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsHovered();
}

bool Button::IsReleased()
{
    return IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && IsHovered();
}

void Button::Draw()
{
    Color fill_color = base.style->fill_default;
    if(IsHovered()) fill_color = base.style->fill_active;
    DrawRectangleRec(base.rect, fill_color);
    if(IsHovered() && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        DrawRectangleLinesEx(base.rect, 2.0f, base.style->outline_active);

    Vector2 label_position = {
        base.rect.x + (base.rect.width - label_size.x)/2.0f,
        base.rect.y + (base.rect.height - label_size.y)/2.0f
    };
    DrawTextEx(*font, label, label_position, font_size, 0.0f, base.style->text_default);
}



TextField::TextField(Style *style, Font *font,
                     int max_character_count, char *default_value,
                     float font_size)
: font(font), default_value(default_value), font_size(font_size)
{
    if(max_character_count < 0) max_character_count = 0;
    if(max_character_count > data_capacity) max_character_count = data_capacity;
    this->max_character_count = max_character_count;
    for(int i = 0; i < max_character_count; i++)
        data[i] = 'X';
    data[max_character_count] = '\0';
    text_size = MeasureTextEx(*font, data, font_size, 0.0f);
    base.min_size.x = text_size.x + 2.0f*style->padding.x;
    base.min_size.y = text_size.y + 2.0f*style->padding.y;
    base.style = style;
    memset(data, 0x0, max_character_count * sizeof(char));
    character_count = 0;
    active = false;
}

bool TextField::IsHovered()
{
    return CheckCollisionPointRec(GetMousePosition(), base.rect);
}

void TextField::Update()
{
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        active = IsHovered();
    }


    if(active)
    {
        int key = GetCharPressed();
        while(key > 0)
        {
            if((key >= 32) && (key <= 125) &&
               (character_count < max_character_count))
            {
                data[character_count] = (char)key;
                data[character_count + 1] = '\0';
                character_count++;
            }

            key = GetCharPressed();
        }

        if(IsKeyPressed(KEY_BACKSPACE))
        {
            character_count--;
            if(character_count < 0) character_count = 0;
            data[character_count] = '\0';
        }
    }
}

void TextField::Draw()
{
    Color fill_color = base.style->fill_default;
    if(active || IsHovered()) fill_color = base.style->fill_active;
    DrawRectangleRec(base.rect, fill_color);
    if(active)
        DrawRectangleLinesEx(base.rect, 2.0f, base.style->outline_active);

    Vector2 label_position = {
        base.rect.x + (base.rect.width - text_size.x)/2.0f,
        base.rect.y + (base.rect.height - text_size.y)/2.0f
    };
    const char *text = character_count > 0 ? data : default_value;
    Color color = character_count > 0 ? base.style->text_default : base.style->text_suggestion;
    DrawTextEx(*font, text, label_position, font_size, 0.0f, color);
}

} // namespace UI