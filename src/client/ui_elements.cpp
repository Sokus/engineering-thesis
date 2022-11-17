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

class Button
{
private:
    const Style *const style;
    const Font *font;
    const char *label;
    int font_size;

public:
    Vector2 label_size;
    Vector2 min_size;
    Rectangle rect;

    Button(const Style *const style, const Font *font, const char *label, int font_size);
    bool IsHovered();
    bool IsPressed();
    bool IsReleased();
    void Draw();
};

Button::Button(const Style *const style, const Font *font, const char *label, int font_size)
: style(style), font(font), label(label), font_size(font_size)
{
    label_size = MeasureTextEx(*font, label, font_size, 0.0f);
    min_size.x = label_size.x + 2.0f*style->padding.x;
    min_size.y = label_size.y + 2.0f*style->padding.y;
}

bool Button::IsHovered()
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
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
    Color fill_color = style->fill_default;
    if(IsHovered()) fill_color = style->fill_active;
    DrawRectangleRec(rect, fill_color);
    if(IsHovered() && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        DrawRectangleLinesEx(rect, 2.0f, style->outline_active);

    Vector2 label_position = {
        rect.x + (rect.width - label_size.x)/2.0f,
        rect.y + (rect.height - label_size.y)/2.0f
    };
    DrawTextEx(*font, label, label_position, font_size, 0.0f, style->text_default);
}