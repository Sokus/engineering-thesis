#include "programs.h"

namespace Game {

    GL::ShaderProgram &grayscaleProgram() {
        static auto result = GL::ShaderProgram::ForPostprocessing(R"glsl(
            #version 330 core
            
            in vec2 v_uv;
            uniform sampler2D tex;
            uniform float strength;
            out vec4 fragColor;

            void main() {
                vec3 color = texture(tex, v_uv).rgb;
                fragColor = vec4(
                    mix(color, vec3(color.r + color.g + color.b) / 3, strength), 
                    1
                );
            }
        )glsl");
        return result;
    }
}