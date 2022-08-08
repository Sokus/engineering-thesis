#ifndef OPENGL3_ENTITY_H
#define OPENGL3_ENTITY_H

#include "opengl3_specialised_renderer.h"

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "opengl3_texture.h"

struct Entity {
    glm::vec2 position; 
    const Texture *texture;
    int layer;
    bool flip;
};

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
    void render(const glm::mat4 &view_projection, const Entity *entities, int count, GLuint light_map);
};

#endif // OPENGL3_ENTITY_H