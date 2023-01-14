#ifndef GRAPHICS_LIGHT_H
#define GRAPHICS_LIGHT_H

#include <glm/glm.hpp>
#include <gl/buffer.h>
#include <gl/vao.h>
#include <gl/shader.h>

namespace Game {

    struct Light {

        glm::vec2 position;  //in world space
        glm::vec3 intensity; //RGB

        // Attenuation coefficients
        float kLinear = 0;
        float kQuadratic = 0;
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