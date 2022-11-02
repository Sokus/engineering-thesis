#include <stddef.h>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "config.h"
#include "opengl3_entity.h"
#include "opengl3_specialised_renderer.h"
#include "opengl3_utils.h"

void EntityRenderer::init()
{
    SpecialisedRenderer::init(6 * sizeof(Vertex));

    glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Vertex), (const void *)offsetof(Vertex, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (const void *)offsetof(Vertex, texture_coordinates));

    for (int i = 0; i < 2; ++i)
        glEnableVertexAttribArray(i);

    Vertex *buffer = (Vertex *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    // NOTE(sokus): Positions and UVs might be the same now,
    // but it might change if we decide to render entities off-center.

    // First Triangle
    //   Lower Left
    buffer[0].position            = glm::vec2(0.0f, 0.0f);
    buffer[0].texture_coordinates = glm::vec2(0.0f, 0.0f);
    //   Lower Right
    buffer[1].position            = glm::vec2(1.0f, 0.0f);
    buffer[1].texture_coordinates = glm::vec2(1.0f, 0.0f);
    //   Upper Right
    buffer[2].position            = glm::vec2(1.0f, 1.0f);
    buffer[2].texture_coordinates = glm::vec2(1.0f, 1.0f);

    // Second Triangle
    //   Lower Right
    buffer[3].position            = glm::vec2(0.0f, 0.0f);
    buffer[3].texture_coordinates = glm::vec2(0.0f, 0.0f);
    //   Upper Right
    buffer[4].position            = glm::vec2(1.0f, 1.0f);
    buffer[4].texture_coordinates = glm::vec2(1.0f, 1.0f);
    //   Upper Left
    buffer[5].position            = glm::vec2(0.0f, 1.0f);
    buffer[5].texture_coordinates = glm::vec2(0.0f, 1.0f);

    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader_program = CreateProgramFromFiles(RESOURCE_PATH "/shaders/entity.vs",
                                            RESOURCE_PATH "/shaders/entity.fs");
}

void EntityRenderer::render(const glm::mat4 &view_projection, const Entity *entities, int count, GLuint light_map)
{
    // TODO: Is this actually neeeded?
    // If it is, find a nicer way to deal with GL_DEPTH_TEST
    bool depth_test_is_enabled = glIsEnabled(GL_DEPTH_TEST);
    if(depth_test_is_enabled)
        glDisable(GL_DEPTH_TEST);

    glBindVertexArray(vao);
    glUseProgram(shader_program);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, light_map);
    glActiveTexture(GL_TEXTURE0);

    SetMat4Uniform(shader_program, "camera_to_clip_projection", view_projection);
    SetIntUniform(shader_program, "texture_array", 0);
    SetIntUniform(shader_program, "light_map", 1);

    GLuint last_texture = 0;
    for(int i=0; i<count; ++i)
    {
        GLuint texture = entities[i].texture->id;
        if(texture != last_texture)
        {
            glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
            last_texture = texture;
        }

        SetIntUniform(shader_program, "layer", entities[i].layer);
        SetBoolUniform(shader_program, "flip", entities[i].flip);

        float scale_x = (float) entities[i].texture->tile_width;
        float scale_y = (float) entities[i].texture->tile_height;
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_x, scale_y, 1.0f));
        glm::mat4 model_to_world = glm::translate(scale, glm::vec3(entities[i].position.x, entities[i].position.y, 0.0f));
        SetMat4Uniform(shader_program, "model_to_world_projection", model_to_world);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    if(depth_test_is_enabled)
        glEnable(GL_DEPTH_TEST);
}