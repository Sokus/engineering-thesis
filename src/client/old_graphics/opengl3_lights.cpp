#include <stddef.h>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/exponential.hpp"
#include "glm/ext/scalar_constants.hpp"

#include "opengl3_lights.h"
#include "macros.h"
#include "opengl3_utils.h"

/* Solves a quadratic equation
 *
 * a,b,c - coefficients of the equation
 * x1,x2 - pointers used to return the results. May be null.
 *
 * If the equation has 1 root, then both x1 and x2 will be set to it.
 *
 * If the equation has 2 roots, then x1 will be set to the smaller and x2
 * to the larger root.
 *
 * Return value is the number of roots.
 */
int solve_quadratic(float a, float b, float c, float *x1, float *x2)
{
    float delta = b*b - 4*a*c;

    if(ABS(a) < 1e-6f)
    {
        if(ABS(b) > 1e-6f)
        {
            if(x1) *x1 = -c/b;
            if(x2) *x2 = -c/b;
            return 1;
        } else return 0;
    }
    else if(delta > 1e-6f)
    {
        if(x1) *x1 = (-b-(float)sqrt(delta))/(2*a);
        if(x2) *x2 = (-b+(float)sqrt(delta))/(2*a);
        return 2;
    }
    else if(delta >= 0)
    {
        if(x1) *x1 = -b/(2*a);
        if(x2) *x2 = -b/(2*a);
        return 1;
    }
    else return 0;
}

float Light::get_range() const
{
    float brightest_component = MAX(MAX(color.r, color.g), color.b);
    float range;
    if(!solve_quadratic(
        k_quadratic * k_bias,
        k_linear * k_bias,
        k_const * k_bias - brightest_component,
        nullptr, &range
    )) range = -1;
    return range;
}

void Light::set_range(float new_range)
{
    float ratio = new_range / get_range();
    if(ratio >= 0)
    {
        k_quadratic /= ratio*ratio;
        k_linear /= ratio;
    }
}

void LightRenderer::init()
{
    SpecialisedRenderer::init(sizeof(Vertex));

    glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Vertex), (const void *) offsetof(Vertex, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (const void *) (offsetof(Vertex, light) + offsetof(Light, position)));
    glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (const void *) (offsetof(Vertex, light) + offsetof(Light, color)));
    glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(Vertex), (const void *) (offsetof(Vertex, light) + offsetof(Light, attenuation)));

    for(int i=0; i<4; ++i) glEnableVertexAttribArray(i);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader_program = CreateProgramFromFiles(RESOURCE_PATH "/shaders/omni.vs",
                                            RESOURCE_PATH "/shaders/omni.fs");
}

void LightRenderer::render(const glm::mat4 &view_projection, const Light *lights, int count, float brightness_multiplier)
{
    constexpr int vertices_per_light = 6;
    ensure_capacity(vertices_per_light * count);

    // Fill the VBO
    Vertex *buffer = (Vertex *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    // We will render a quad for each light
    // The size of the quad will be determined by the maximum range of the light
    // (how far the light can reach)
    glm::vec2 position_lookup[vertices_per_light] = {
        glm::vec2(-1,-1), glm::vec2(1,-1), glm::vec2(-1,1),
        glm::vec2(-1,1), glm::vec2(1,-1), glm::vec2(1,1)
    };

    int no_vertices_written = 0;
    for(int i=0; i<count; ++i)
    {
        Light light = lights[i];
        float range = light.get_range();

        // Skip lights with range <= 0
        if(range <= 0)
            continue;

        for(int j=0; j<vertices_per_light; ++j)
        {
            glm::vec2 vertex_pos = position_lookup[j] * glm::vec2(range) + light.position;
            buffer[no_vertices_written++] = {vertex_pos, light};
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Render the lights
    glBindVertexArray(vao);
    glUseProgram(shader_program);
    SetMat4Uniform(shader_program, "view_projection", view_projection);
    SetFloatUniform(shader_program, "brightness_multiplier", brightness_multiplier);
    glBlendFunc(GL_ONE, GL_ONE);
    glDrawArrays(GL_TRIANGLES, 0, no_vertices_written);

    //Cleanup
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(0);
    glUseProgram(0);
}
