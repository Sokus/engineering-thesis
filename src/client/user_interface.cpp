#include "user_interface.h"

#include <string.h>

namespace UI {

Layout layout;

void Init()
{
    Style *style = &layout.default_style;

    style->font = LoadFontEx(RESOURCE_PATH "/LiberationMono-Regular.ttf", 96, 0, 0);
    SetTextureFilter(style->font.texture, TEXTURE_FILTER_BILINEAR);
    style->font_size = 30;
    style->fill_default = LIGHTGRAY;
    style->fill_active = WHITE;
    style->text_default = DARKGRAY;
    style->text_suggestion = GRAY;
    style->outline_active = RED;
    style->padding = {10.0f, 10.0f};
    style->spacing = {4.0f, 4.0f};
    layout.current_style = style;
}

void SetDefaultStyle(Style *style)
{
    memcpy(&layout.default_style, style, sizeof(Style));
}

Style *GetCurrentStyle()
{
    return layout.current_style;
}

void SetPosition(float x, float y)
{
    layout.position = Vector2{ x, y };
}

void SetOrigin(float x, float y)
{
    layout.origin = Vector2{ x, y };
}

void Begin()
{
    layout.element_count = 0;
    layout.row_count = 0;
    memset(layout.rows, 0, sizeof(layout.rows));
    layout.min_size = Vector2{ 0.0f, 0.0f };
    layout.element_offset = Vector2{ 0.0f, 0.0f };
}

void Add(Base *base)
{
    if(layout.element_count >= layout.max_element_count) return;
    if(layout.row_count >= layout.max_row_count) return;

    layout.elements[layout.element_count] = base;
    float width = base->min_size.x + 2.0f * base->style->spacing.x;
    float height = base->min_size.y + 2.0f * base->style->spacing.y;
    LayoutRow *current_row = layout.rows + layout.row_count;
    current_row->min_size.x += width;
    if(height > current_row->min_size.y)
        current_row->min_size.y = height;
    current_row->count++;
    layout.element_count++;
}

void EndRow()
{
    if(layout.row_count >= layout.max_row_count) return;

    LayoutRow *current_row = layout.rows + layout.row_count;
    if (current_row->min_size.x > layout.min_size.x)
        layout.min_size.x = current_row->min_size.x;
    layout.min_size.y += current_row->min_size.y;

    layout.row_count++;
    current_row = layout.rows + layout.row_count;
    current_row->start = layout.element_count;
    current_row->count = 0;
    current_row->min_size = Vector2{ 0.0f, 0.0f };
}

void End()
{
    for(int row_idx = 0; row_idx < layout.row_count; row_idx++)
    {
        LayoutRow *row = layout.rows + row_idx;
        layout.element_offset.x = 0.0f;
        float free_width = layout.min_size.x - row->min_size.x;
        for(int element_idx = row->start;
            element_idx < row->start + row->count;
            element_idx++)
        {
            Base *element = layout.elements[element_idx];
            float element_width = element->min_size.x + 2.0f*element->style->spacing.x;
            float width_ratio = element_width / row->min_size.x;
            float added_width = width_ratio * free_width;
            float origin_position_x = layout.position.x - layout.origin.x * layout.min_size.x;
            float origin_position_y = layout.position.y - layout.origin.y * layout.min_size.y;
            element->rect.x = origin_position_x + layout.element_offset.x + element->style->spacing.x;
            element->rect.width = element->min_size.x + added_width;
            layout.element_offset.x += element_width + added_width;
            element->rect.y = origin_position_y + layout.element_offset.y + element->style->spacing.y;
            element->rect.height = row->min_size.y - 2.0f*element->style->spacing.y;
        }
        layout.element_offset.y += row->min_size.y;
    }
}

Button::Button(char *label)
: label(label)
{
    base.style = layout.current_style;
    label_size = MeasureTextEx(base.style->font, label, base.style->font_size, 0.0f);
    base.min_size.x = label_size.x + 2.0f*base.style->padding.x;
    base.min_size.y = label_size.y + 2.0f*base.style->padding.y;
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

    DrawTextEx(base.style->font, label, label_position,
               base.style->font_size, 0.0f, base.style->text_default);
}

TextField::TextField(int max_character_count, char *default_value)
: default_value(default_value)
{
    if(max_character_count < 0) max_character_count = 0;
    if(max_character_count > data_capacity) max_character_count = data_capacity;
    this->max_character_count = max_character_count;
    for(int i = 0; i < max_character_count; i++)
        data[i] = 'X';
    data[max_character_count] = '\0';
    base.style = layout.current_style;
    text_size = MeasureTextEx(base.style->font, data, base.style->font_size, 0.0f);
    base.min_size.x = text_size.x + 2.0f*base.style->padding.x;
    base.min_size.y = text_size.y + 2.0f*base.style->padding.y;
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
        active = IsHovered();

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
    DrawTextEx(base.style->font, text, label_position, base.style->font_size, 0.0f, color);
}

} // namespace UI