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

    GL::ShaderProgram &toneMappingProgram() {
        static auto result = GL::ShaderProgram::ForPostprocessing(R"glsl(
            #version 330 core

            in vec2 v_uv;
            uniform sampler2D tex;
            uniform float exposure;
            out vec4 fragColor;

            void main() {
                vec3 color = texture(tex, v_uv).rgb;
                color = color / (color + vec3(1.0));
                color = pow(color, vec3(1/exposure));
                fragColor = vec4(color, 1.0);
            }
        )glsl");
        return result;
    }

    GL::ShaderProgram &vignetteProgram() {
        static auto result = GL::ShaderProgram::ForPostprocessing(R"glsl(
            #version 330 core
            
            in vec2 v_uv;
            uniform vec4 vignetteColor;
            uniform float vignetteExponent;
            out vec4 fragColor;

            float product(vec2 v) {
                return v.x * v.y;
            }

            void main() {

                float strength = 1 - pow(
                    16*product(v_uv * (1.0-v_uv)),
                    vignetteExponent
                );

                fragColor = vec4(vignetteColor.rgb, vignetteColor.a * strength);
            }
        )glsl");
        return result;
    }

    GL::ShaderProgram &blur3x3Program() {
        static auto result = GL::ShaderProgram::ForPostprocessing(R"glsl(
            #version 330 core

            in vec2 v_uv;
            uniform sampler2D tex;
            out vec4 fragColor;

            void main() {
                vec2 d_uv = vec2(1.0) / textureSize(tex,0);
                vec3 result = vec3(0.0);
                result += texture(tex, v_uv + d_uv * vec2(-1,-1)).rgb;
                result += texture(tex, v_uv + d_uv * vec2(-1, 1)).rgb;
                result += texture(tex, v_uv + d_uv * vec2( 1,-1)).rgb;
                result += texture(tex, v_uv + d_uv * vec2( 1, 1)).rgb;
                result += 2*texture(tex, v_uv + d_uv * vec2( 0,-1)).rgb;
                result += 2*texture(tex, v_uv + d_uv * vec2( 0, 1)).rgb;
                result += 2*texture(tex, v_uv + d_uv * vec2(-1, 0)).rgb;
                result += 2*texture(tex, v_uv + d_uv * vec2( 1, 0)).rgb;
                result += 4*texture(tex, v_uv).rgb;
                result *= 1.0 / 16.0;
                fragColor = vec4(result, 1.0);
            }
        )glsl");
        return result;
    }

    GL::ShaderProgram &bloomApplyProgram() {
        static auto result = GL::ShaderProgram::ForPostprocessing(R"glsl(
            #version 330 core

            in vec2 v_uv;
            uniform sampler2D tex;
            uniform float bloomStrength;
            out vec4 fragColor;

            void main() {
                fragColor = vec4(texture(tex,v_uv).rgb, bloomStrength);
            }
        )glsl");
        return result;
    }
}