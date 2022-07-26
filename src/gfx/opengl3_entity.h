#ifndef OPENGL3_ENTITY_H
#define OPENGL3_ENTITY_H

#include "opengl3_specialised_renderer.h"

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "opengl3_texture.h"

struct EntityRenderer : public SpecialisedRenderer
{
    private:
    struct Vertex
    {
        glm::vec2 position;
        glm::vec2 texture_coordinates;
    };

    public:
    void init();
    void render(float pos_x, float pos_y, const Texture& texture, int layer, bool flip);
    void update_view(const glm::mat4 &view_projection);

    GLuint TEMPORARY_get_shader_program();
};

#endif // OPENGL3_ENTITY_H