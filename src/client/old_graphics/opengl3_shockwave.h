#ifndef OPENGL3_SHOCKWAVE_H
#define OPENGL3_SHOCKWAVE_H

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "opengl3_specialised_renderer.h"

struct Shockwave
{
    glm::vec2 center; //where the shockwave started
    float radius; //distance from center to the current wave position
    float scale; //how wide is the shockwave
    float strength; //how strongly the shockwave distorts the image
};

struct ShockwaveRenderer : public SpecialisedRenderer
{
    private:
    struct Vertex
    {
        glm::vec2 position;
        Shockwave shockwave;
    };

    public:
    void init();
    void render(const glm::mat4 &view_projection, GLuint src_texture,
                const Shockwave *shockwaves, int count);
};

#endif // OPENGL3_SHOCKWAVE_H