#include "distortion.h"

#include <gl/shader.h>
#include <gl/gl.h>

namespace Game {

    GL::ShaderProgram &shockwaveProgram() {
        static GL::ShaderProgram result(
            R"glsl(
                #version 330 core

                layout(location=0) in vec2 position;
                layout(location=1) in vec2 center;
                layout(location=2) in float radius;
                layout(location=3) in float scale;
                layout(location=4) in float strength;

                uniform mat4 view_projection;

                out vec2 v_uv;
                out vec2 v_position;
                out vec2 v_center;
                out float v_radius;
                out float v_scale;
                out float v_strength;

                void main()
                {
                    gl_Position = view_projection * vec4(position, 0.0, 1.0);
                    v_uv = (gl_Position.xy + 1) / 2;
                    v_position = position;
                    v_center = center;
                    v_radius = radius;
                    v_scale = scale;
                    v_strength = strength;
                }
            )glsl",
            R"glsl(
                #version 330 core
                in vec2 v_uv;
                in vec2 v_position;
                in vec2 v_center;
                in float v_radius;
                in float v_scale;
                in float v_strength;

                const float bias = exp(-4.0);

                uniform sampler2D tex;

                out vec4 frag_color;

                void main()
                {
                    vec2 displacement = v_position - v_center;
                    float dist_from_center = length(displacement);
                    vec2 pixel_size = vec2(1)/vec2(textureSize(tex,0).xy);

                    float arg = (dist_from_center - v_radius)/v_scale;
                    float alpha = max(0, exp(-arg*arg) - bias);

                    vec2 uv_offset = normalize(displacement)*alpha*v_strength*pixel_size*vec2(-1,1);

                    frag_color = vec4(
                        texture(tex, v_uv + uv_offset).rgb,
                        alpha
                    );
                }
            )glsl"
        );
        return result;
    }

    DistortionRenderer::DistortionRenderer() {
        vbo.Bind();
        vao.Bind();
        glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Vertex), (const void *)offsetof(Vertex, position));
        glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (const void *)(offsetof(Vertex, shockwave) + offsetof(ShockwaveEffect, center)));
        glVertexAttribPointer(2, 1, GL_FLOAT, false, sizeof(Vertex), (const void *)(offsetof(Vertex, shockwave) + offsetof(ShockwaveEffect, radius)));
        glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(Vertex), (const void *)(offsetof(Vertex, shockwave) + offsetof(ShockwaveEffect, scale)));
        glVertexAttribPointer(4, 1, GL_FLOAT, false, sizeof(Vertex), (const void *)(offsetof(Vertex, shockwave) + offsetof(ShockwaveEffect, strength)));

        for (int i = 0; i < 5; ++i)
            glEnableVertexAttribArray(i);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void DistortionRenderer::DrawDistortions(
        const glm::mat4 &viewProjection,
        GL::TextureUnit sourceTexture, 
        const ShockwaveEffect* shockwaves, int noShockwaves
    ) {
        // Generate vertex data
        constexpr int vertices_per_shockwave = 6;
        glm::vec2 position_lookup[6] = {
            glm::vec2(-1, -1), glm::vec2(1, -1), glm::vec2(-1, 1),
            glm::vec2(-1, 1), glm::vec2(1, -1), glm::vec2(1, 1)
        };
        vertices.clear();
        vertices.reserve(vertices_per_shockwave*noShockwaves);

        for(
            const ShockwaveEffect *shockwave = shockwaves; 
            shockwave < shockwaves+noShockwaves; 
            ++shockwave
        ) {
            float bounding_circle_radius = shockwave->radius + 2 * shockwave->scale;

            if (bounding_circle_radius <= 0)
                continue;

            for (int j = 0; j < vertices_per_shockwave; ++j) {
                glm::vec2 vertex_pos = position_lookup[j] * glm::vec2(bounding_circle_radius) + shockwave->center;
                vertices.push_back(Vertex{vertex_pos, *shockwave});
            }
        }

        vbo.Upload(vertices.data(), vertices.size());
        vao.Bind();

        shockwaveProgram()
            .SetUniform("view_projection", viewProjection)
            .SetUniform("tex", sourceTexture);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }
}