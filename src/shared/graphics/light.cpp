#include "light.h"

#include <algorithm>
#include <gl/shader.h>
#include <stdio.h>

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
int SolveQuadratic(float a, float b, float c, float *x1, float *x2)
{
    float delta = b*b - 4*a*c;

    if(std::abs(a) < 1e-6f)
    {
        if(std::abs(b) > 1e-6f)
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

namespace Game {

    float Light::Range() const {

        float brightest_component = std::max(intensity.r, std::max(intensity.g, intensity.b));
        float range;
        if(!SolveQuadratic(
            kQuadratic * kBias,
            kLinear * kBias,
            kBias - brightest_component,
            nullptr, &range
        )) range = 0;
        return range;
    }
    void Light::SetRange(float newRange) {
        float ratio = newRange / Range();
        if(ratio > 0) {
            kQuadratic /= ratio*ratio;
            kLinear /= ratio;
        }
    }

    LightRenderer::LightRenderer() {
        
        vao.Bind();
        vbo.Bind();

        glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(LightVertex), (const void *)offsetof(LightVertex,position));
        glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(LightVertex), (const void *)offsetof(LightVertex,lightPosition));
        glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(LightVertex), (const void *)offsetof(LightVertex,intensity));
        glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(LightVertex), (const void *)offsetof(LightVertex,attenuation));

        for(int i=0; i<4; ++i) glEnableVertexAttribArray(i);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    GL::ShaderProgram &ambientLightProgram() {
        static auto result = GL::ShaderProgram::ForPostprocessing(
            R"glsl(
                #version 330 core

                in vec2 v_uv;
                uniform sampler2D albedoMap;
                uniform vec3 ambientLight;
                out vec3 fragColor;

                void main() {
                    fragColor = texture(albedoMap, v_uv).rgb * ambientLight;
                }
            )glsl"
        );
        return result;
    }

    GL::ShaderProgram &omniLightProgram() {
        static GL::ShaderProgram result(
            R"glsl(
                #version 330 core

                layout(location=0) in vec2 position;
                layout(location=1) in vec2 light_position;
                layout(location=2) in vec3 light_color;
                layout(location=3) in vec4 attenuation;

                uniform mat4 view_projection;

                out vec2 v_uv;
                out vec2 v_position;
                out vec2 v_light_position;
                out vec3 v_light_color;
                out vec4 v_attenuation;

                void main() {
                    vec4 clipSpacePosition = view_projection * vec4(position, 0.0, 1.0);
                    gl_Position = clipSpacePosition;
                    v_uv = (clipSpacePosition.xy + vec2(1.0)) * 0.5;
                    v_position = position;
                    v_light_position = light_position;
                    v_light_color = light_color;
                    v_attenuation = attenuation;
                }
            )glsl",
            R"glsl(
                #version 330 core

                in vec2 v_uv;
                in vec2 v_position;
                in vec2 v_light_position;
                in vec3 v_light_color;
                in vec4 v_attenuation;

                float k_quadratic() {return v_attenuation.x;}
                float k_linear()    {return v_attenuation.y;}
                float k_bias()      {return v_attenuation.z;}

                uniform sampler2D albedoMap;
                uniform sampler2D depthMap;
                uniform float bleedFactor;

                out vec3 frag_color;

                vec3 intensityAt(vec3 where) {
                    float d = distance(
                        where, 
                        vec3(v_light_position, 1.0)
                    );
                    return max(
                        vec3(0.0), 
                        v_light_color / (k_quadratic()*d*d + k_linear()*d + 1.0) - vec3(k_bias())
                    );
                }

                void main() {
                    float depth = texture(depthMap, v_uv).r;
                    vec3 albedo = texture(albedoMap, v_uv).rgb;
                    
                    frag_color = 
                        intensityAt(vec3(v_position, depth)) * albedo * (1-bleedFactor) +
                        intensityAt(vec3(v_position, 1.0)) * bleedFactor;
                }
            )glsl"
        );
        return result;
    }

    IndexInterval LightRenderer::PrepareVertexData(const Light *lights, int count) {

        auto oldVertexCount = vertices.size();

        glm::vec2 positionLookup[6] = {
            glm::vec2(-1,-1), glm::vec2(1,-1), glm::vec2(-1,1),
            glm::vec2(-1,1), glm::vec2(1,-1), glm::vec2(1,1)
        };

        for(const Light *light = lights; light < lights+count; ++light) {

            float range = light->Range();

            if(range <= 0) 
                continue;

            for(int i=0; i<6; ++i) {
                vertices.push_back({
                    positionLookup[i] * glm::vec2(range) + light->position,
                    light->position,
                    light->intensity,
                    light->attenuation
                });
            }
        }

        return {(int)oldVertexCount, (int)(vertices.size() - oldVertexCount)};
    }

    void LightRenderer::DrawLights(
        const glm::mat4 &viewProjection, 
        GL::TextureUnit albedoMap, GL::TextureUnit depthMap,
        const glm::vec3 &ambientLight, 
        const Light *lights, int noLights,
        const Light *energySpheres, int noEnergySpheres
    ) {
        // Setup
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);

        vertices.clear();
        vertices.reserve(6 * (noLights+noEnergySpheres));

        // Ambient light
        ambientLightProgram()
            .SetUniform("albedoMap", albedoMap)
            .SetUniform("ambientLight", ambientLight)
            .DrawPostprocessing();

        // Omni lights
        auto lightIndices = PrepareVertexData(lights, noLights);
        auto energySphereIndices = PrepareVertexData(energySpheres, noEnergySpheres);

        vbo.Upload(vertices.data(), vertices.size());
        vao.Bind();

        omniLightProgram()
            .SetUniform("albedoMap", albedoMap)
            .SetUniform("depthMap", depthMap)
            .SetUniform("view_projection", viewProjection)
            .SetUniform("bleedFactor", 0.0f);
        glDrawArrays(GL_TRIANGLES, lightIndices.first, lightIndices.count);

        omniLightProgram().SetUniform("bleedFactor", 1.0f);
        glDrawArrays(GL_TRIANGLES, energySphereIndices.first, energySphereIndices.count);

        // Cleanup
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}