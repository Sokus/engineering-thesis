#include "shader.h"
#include <rlgl.h>
#include <glm/gtc/type_ptr.hpp>

namespace GL {

    ShaderProgram::ShaderProgram(const char *vertexSource, const char *fragmentSource) {
        handle = rlLoadShaderCode(vertexSource, fragmentSource);
    }
    ShaderProgram::~ShaderProgram() {
        rlUnloadShaderProgram(handle);
    }
    void ShaderProgram::Bind() {
        glUseProgram(handle);
    }
    ShaderProgram ShaderProgram::ForPostprocessing(const char *fragmentSource) {
        return ShaderProgram(
            R"glsl(
                // Base vertex shader for postprocessing
                #version 330 core

                out vec2 v_uv;

                void main() 
                {
                    // We just want to render a fullscreen quad,
                    // so we hardcode the model into the shader for simplicity.
                    vec2 corners[] = vec2[6](
                        vec2(-1,-1),
                        vec2( 1,-1),
                        vec2(-1, 1),
                        vec2(-1, 1),
                        vec2( 1,-1),
                        vec2( 1, 1)
                    );
                    gl_Position = vec4(corners[gl_VertexID], 0, 1);
                    v_uv = (corners[gl_VertexID]+1) / 2;
                }
            )glsl",
            fragmentSource
        );
    }
    void ShaderProgram::DrawPostprocessing() {

        static GLuint dummyVAO = 0;
        if(!dummyVAO) glGenVertexArrays(1, &dummyVAO);
        glBindVertexArray(dummyVAO);

        Bind();

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    ShaderProgram &ShaderProgram::SetUniform(const char *name, int value) {
        Bind();
        glUniform1i(glGetUniformLocation(handle, name), value);
        return *this;
    }
    ShaderProgram &ShaderProgram::SetUniform(const char *name, const TextureUnit &value) {
        return SetUniform(name, static_cast<int>(value.Index()));
    }
    ShaderProgram &ShaderProgram::SetUniform(const char *name, float value) {
        Bind();
        glUniform1f(glGetUniformLocation(handle, name), value);
        return *this;
    }
    ShaderProgram &ShaderProgram::SetUniform(const char *name, const glm::vec3 &value) {
        Bind();
        glUniform3f(glGetUniformLocation(handle,name), value.x, value.y, value.z);
        return *this;
    }
    ShaderProgram &ShaderProgram::SetUniform(const char *name, const glm::vec4 &value) {
        Bind();
        glUniform4f(glGetUniformLocation(handle,name), value.x, value.y, value.z, value.w);
        return *this;
    }
    ShaderProgram &ShaderProgram::SetUniform(const char *name, const glm::mat4 &value) {
        Bind();
        glUniformMatrix4fv(glGetUniformLocation(handle, name), 1, GL_FALSE, glm::value_ptr(value));
        return *this;
    }
}