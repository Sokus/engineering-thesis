#ifndef GRAPHICS_PROGRAMS_H
#define GRAPHICS_PROGRAMS_H

#include <gl/shader.h>

namespace Game {
    GL::ShaderProgram &grayscaleProgram();
    GL::ShaderProgram &toneMappingProgram();
}

#endif