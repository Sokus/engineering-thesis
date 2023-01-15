#ifndef GRAPHICS_LIGHT_H
#define GRAPHICS_LIGHT_H

#include <vector>
#include <glm/glm.hpp>
#include <gl/buffer.h>
#include <gl/vao.h>
#include <gl/texture.h>

namespace Game {

    struct Light {

        glm::vec2 position;  //in world space
        glm::vec3 intensity = {1.0f, 1.0f, 1.0f}; //RGB

        // Attenuation coefficients
        union {
            struct {
                float kQuadratic;
                float kLinear;
                float kBias;
                float _kReserved;
            };
            glm::vec4 attenuation = {6.0f, 3.0f, 0.1f, 0.5f};
        };

        float Range() const;
        void SetRange(float newRange);
    };

    struct LightVertex {
        glm::vec2 position;
        glm::vec2 lightPosition;
        glm::vec3 intensity;
        glm::vec4 attenuation;
    };

    struct LightRenderer {

        private:
        GL::Buffer vbo;
        GL::VAO vao;
        std::vector<LightVertex> vertices;


        public:

        LightRenderer();

        void DrawLights(
            const glm::mat4 &viewProjection, 
            GL::TextureUnit albedoMap,
            const glm::vec3 &ambientLight, const Light *lights, int count
        );
    };

}

#endif