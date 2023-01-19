#ifndef GRAPHICS_DISTORTION_H
#define GRAPHICS_DISTORTION_H

#include <gl/buffer.h>
#include <gl/vao.h>
#include <gl/texture.h>
#include <glm/glm.hpp>
#include <vector>

namespace Game {

    struct ShockwaveEffect {
        glm::vec2 center;
        float radius;
        // Thickness of the shockwave
        float scale;
        // How much the image is distorted by the shockwave
        float strength;
    };

    struct DistortionRenderer {
        private:

        struct Vertex {
            glm::vec2 position;
            ShockwaveEffect shockwave;
        };

        GL::Buffer vbo;
        GL::VAO vao;
        std::vector<Vertex> vertices;

        public:
        DistortionRenderer();

        void DrawDistortions(
            const glm::mat4 &viewProjection,
            GL::TextureUnit sourceTexture, 
            const ShockwaveEffect* shockwaves, int noShockwaves
        );
    };
}

#endif