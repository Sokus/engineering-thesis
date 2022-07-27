#include "opengl3_draw_queue.h"

DrawQueue::DrawQueue() 
{
    entities.reserve(256);
    lights.reserve(256);
    shockwaves.reserve(256);
}
    
void DrawQueue::Clear()
{
    entities.clear();
    lights.clear();
    shockwaves.clear();
}

void DrawQueue::SetViewportDimensions(const glm::ivec2 &value)
{
    viewport_dimensions = value;
}
const glm::ivec2 &DrawQueue::GetViewportDimensions() const
{
    return viewport_dimensions;
}

void DrawQueue::SetProjectionMatrix(const glm::mat4 &value)
{
    projection_matrix = value;
}
const glm::mat4 &DrawQueue::GetProjectionMatrix() const
{
    return projection_matrix;
}

void DrawQueue::SetViewMatrix(const glm::mat4 &value)
{
    view_matrix = value;
}
const glm::mat4 &DrawQueue::GetViewMatrix() const
{
    return view_matrix;
}

void DrawQueue::SetBackgroundColor(const glm::vec3 &value)
{
    background_color = value;
}
const glm::vec3 &DrawQueue::GetBackgroundColor() const
{
    return background_color;
}

void DrawQueue::SetAmbientLightColor(const glm::vec3 &value)
{
    ambient_light_color = value;
}
const glm::vec3 &DrawQueue::GetAmbientLightColor() const
{
    return ambient_light_color;
}

void DrawQueue::AddLight(const Light &light)
{
    lights.push_back(light);
}
void DrawQueue::AddShockwave(const Shockwave &shockwave)
{
    shockwaves.push_back(shockwave);
}
void DrawQueue::AddEntity(glm::vec2 position, const Texture &texture, int layer, bool flip)
{
    entities.push_back({position, &texture, layer, flip});
}