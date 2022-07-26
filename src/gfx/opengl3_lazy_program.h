#ifndef OPENGL3_LAZY_PROGRAM_H
#define OPENGL3_LAZY_PROGRAM_H

class LazyProgram
{
    GLuint _handle = 0;
    const char *vertex_shader_path, *fragment_shader_path;

    public:
    LazyProgram(const LazyProgram &) = delete;
    LazyProgram(const char *vertex_shader_path, const char *fragment_shader_path) :
        vertex_shader_path(vertex_shader_path),
        fragment_shader_path(fragment_shader_path)
    {}

    operator GLuint() {
        if(!_handle)
            _handle = CreateProgramFromFiles(vertex_shader_path, fragment_shader_path);
        return _handle;
    }
};

#endif // OPENGL3_LAZY_PROGRAM_H