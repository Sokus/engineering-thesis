#include "shader.h"
#include <rlgl.h>

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
}