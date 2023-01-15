#ifndef GRAPHICS_LIGHT_H
#define GRAPHICS_LIGHT_H

#include <glm/glm.hpp>
#include <gl/buffer.h>
#include <gl/vao.h>
#include <gl/shader.h>

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

    struct LightRenderer {

        private:
        GL::Buffer vbo;
        GL::VAO vao;
        //GL::ShaderProgram program;

        public:
        LightRenderer();
    };

}

#endif