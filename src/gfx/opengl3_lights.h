#ifndef OPENGL3_LIGHTS_H
#define OPENGL3_LIGHTS_H

#include "glm/glm.hpp"
#include "opengl3_specialised_renderer.h"

struct Light
{
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    /* Formula for light intensity:
       max(0, color / (k_quadratic*d*d + k_linear*d + k_const) - k_bias)
       where d=distance from center
    */
    union
    {
        glm::vec4 attenuation = glm::vec4(6.0f, 3.0f, 1.0f, 0.1f);
        struct {
            float k_quadratic, k_linear, k_const, k_bias;
        };
    };

    float get_range() const;
    void set_range(float new_range);
};

class LightRenderer : public SpecialisedRenderer
{
    private:

    struct Vertex
    {
        glm::vec2 position;
        Light light;
    };

    public:

    void init();
    void render(const glm::mat4 &view_projection, const Light *lights, int count);
};

#endif