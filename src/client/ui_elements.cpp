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
    float font_size;

public:
    Vector2 label_size;
    Vector2 min_size;
    Rectangle rect;

    Button(const Style *const style, const Font *font, const char *label, float font_size);
    bool IsHovered();
    bool IsPressed();
    bool IsReleased();
    void Draw();
};

Button::Button(const Style *const style, const Font *font, const char *label, float font_size)
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

class TextField
{
private:
    const Style *const style;
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
    Vector2 text_size;
    Vector2 min_size;
    Rectangle rect;

    TextField(const Style *const style, const Font *font,
              int max_character_count, const char *default_value,
              float font_size);
    bool IsHovered();
    void Update();
    void Draw();
};

TextField::TextField(const Style *const style, const Font *font,
                     int max_character_count, const char *default_value,
                     float font_size)
: style(style), font(font), default_value(default_value), font_size(font_size)
{
    if(max_character_count < 0) max_character_count = 0;
    if(max_character_count > data_capacity) max_character_count = data_capacity;
    this->max_character_count = max_character_count;
    for(int i = 0; i < max_character_count; i++)
        data[i] = 'X';
    data[max_character_count] = '\0';
    text_size = MeasureTextEx(*font, data, font_size, 0.0f);
    min_size.x = text_size.x + 2.0f*style->padding.x;
    min_size.y = text_size.y + 2.0f*style->padding.y;
    memset(data, 0x0, max_character_count * sizeof(char));
    character_count = 0;
    active = false;
}

bool TextField::IsHovered()
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
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
    Color fill_color = style->fill_default;
    if(active || IsHovered()) fill_color = style->fill_active;
    DrawRectangleRec(rect, fill_color);
    if(active)
        DrawRectangleLinesEx(rect, 2.0f, style->outline_active);

    Vector2 label_position = {
        rect.x + (rect.width - text_size.x)/2.0f,
        rect.y + (rect.height - text_size.y)/2.0f
    };
    const char *text = character_count > 0 ? data : default_value;
    Color color = character_count > 0 ? style->text_default : style->text_suggestion;
    DrawTextEx(*font, text, label_position, font_size, 0.0f, color);
}