#ifndef OPENGL3_UTILS_H
#define OPENGL3_UTILS_H

const char *StringifyGLenum(GLenum value);

GLuint CreateProgram(const char *vertex_shader_source,
                     const char *fragment_shader_source);
GLuint CreateProgramFromFiles(const char *vertex_shader_path,
                              const char *fragment_shader_path);
void SetBoolUniform(GLuint program, const char *name, bool value);
void SetIntUniform(GLuint program, const char *name, int value);
void SetFloatUniform(GLuint program, const char *name, float value);
void SetVec2Uniform(GLuint program, const char *name, glm::vec2 value);
void SetMat4Uniform(GLuint program, const char *name, const glm::mat4& matrix);
void SetFloatArrayUniform(GLuint program, const char *name, const float *array, int count);

#endif // OPENGL3_UTILS_H