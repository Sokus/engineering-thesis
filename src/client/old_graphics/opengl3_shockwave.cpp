#include <stddef.h>

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "opengl3_shockwave.h"
#include "opengl3_specialised_renderer.h"
#include "config.h"
#include "opengl3_utils.h"

void ShockwaveRenderer::init()
{
    SpecialisedRenderer::init(sizeof(Vertex));

    glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Vertex), (const void *)offsetof(Vertex, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (const void *)(offsetof(Vertex, shockwave) + offsetof(Shockwave, center)));
    glVertexAttribPointer(2, 1, GL_FLOAT, false, sizeof(Vertex), (const void *)(offsetof(Vertex, shockwave) + offsetof(Shockwave, radius)));
    glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(Vertex), (const void *)(offsetof(Vertex, shockwave) + offsetof(Shockwave, scale)));
    glVertexAttribPointer(4, 1, GL_FLOAT, false, sizeof(Vertex), (const void *)(offsetof(Vertex, shockwave) + offsetof(Shockwave, strength)));

    for (int i = 0; i < 5; ++i)
        glEnableVertexAttribArray(i);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader_program = CreateProgramFromFiles(RESOURCE_PATH "/shaders/shockwave.vs",
                                            RESOURCE_PATH "/shaders/shockwave.fs");
}

void ShockwaveRenderer::render(const glm::mat4 &view_projection, GLuint src_texture,
                               const Shockwave *shockwaves, int count)
{
    constexpr int vertices_per_shockwave = 6;
    ensure_capacity(vertices_per_shockwave * count);

    Vertex *buffer = (Vertex *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    glm::vec2 position_lookup[vertices_per_shockwave] = {
        glm::vec2(-1, -1), glm::vec2(1, -1), glm::vec2(-1, 1),
        glm::vec2(-1, 1), glm::vec2(1, -1), glm::vec2(1, 1)};

    int no_vertices_written = 0;
    for (int i = 0; i < count; ++i)
    {
        Shockwave shockwave = shockwaves[i];
        float bounding_circle_radius = shockwave.radius + 2 * shockwave.scale;

        if (bounding_circle_radius <= 0)
            continue;

        for (int j = 0; j < vertices_per_shockwave; ++j)
        {
            glm::vec2 vertex_pos = position_lookup[j] * glm::vec2(bounding_circle_radius) + shockwave.center;
            buffer[no_vertices_written++] = {vertex_pos, shockwave};
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindVertexArray(vao);
    glUseProgram(shader_program);
    SetMat4Uniform(shader_program, "view_projection", view_projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, src_texture);
    SetIntUniform(shader_program, "tex", 0);
    glDrawArrays(GL_TRIANGLES, 0, no_vertices_written);

    glBindVertexArray(0);
    glUseProgram(0);
}