#ifndef GRAPHICS_LIGHT_H
#define GRAPHICS_LIGHT_H

#include <vector>
#include <glm/glm.hpp>
#include <gl/buffer.h>
#include <gl/vao.h>
#include <gl/texture.h>

namespace Game {

    struct IndexInterval {
        int first;
        int count;
    };

    struct Light {

        glm::vec2 position;  //in world space
        glm::vec3 intensity = {1.0f, 1.0f, 1.0f}; //RGB

        union {
            struct {
                float kQuadratic;
                float kLinear;
                float kBias;
                float _kReserved;
            };
            glm::vec4 attenuation = {3.0f, 0.0f, 0.1f, 0.5f};
        };

        float Range() const;
        void SetRange(float newRange);

        static Light CreateRanged(const glm::vec2 &position, const glm::vec3 &intensity, float range);
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

        IndexInterval PrepareVertexData(const Light *lights, int count);

        public:

        LightRenderer();

        void DrawLights(
            const glm::mat4 &viewProjection, 
            GL::TextureUnit albedoMap,
            const glm::vec3 &ambientLight, 
            const Light *lights, int noLights,
            const Light *energySpheres, int noEnergySpheres
        );
    };

}

#endif