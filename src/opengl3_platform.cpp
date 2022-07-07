GLuint CreateProgram(const char *vertex_shader_source,
                     const char *fragment_shader_source)
{
    int success;
    char info_log[512];
    
    GLuint vertex_shader_handle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_handle, 1, (const char * const *)&vertex_shader_source, 0);
    glCompileShader(vertex_shader_handle);
    glGetShaderiv(vertex_shader_handle, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader_handle, 512, 0, info_log);
        fprintf(stderr, "ERROR: (vertex compile) %s\n", info_log);
    }
    
    GLuint fragment_shader_handle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_handle, 1, (const char * const *)&fragment_shader_source, 0);
    glCompileShader(fragment_shader_handle);
    glGetShaderiv(fragment_shader_handle, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader_handle, 512, 0, info_log);
        fprintf(stderr, "ERROR: (fragment compiler) %s\n", info_log);
    }
    
      GLuint program_handle = glCreateProgram();
    glAttachShader(program_handle, vertex_shader_handle);
    glAttachShader(program_handle, fragment_shader_handle);
    glLinkProgram(program_handle);
    glGetProgramiv(program_handle, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(program_handle, 512, 0, info_log);
        fprintf(stderr, "ERROR: (program link) %s\n", info_log);
    }
    
    glDetachShader(program_handle, vertex_shader_handle);
    glDeleteShader(vertex_shader_handle);
    glDetachShader(program_handle, fragment_shader_handle);
    glDeleteShader(fragment_shader_handle);
    
    return program_handle;
}

void SetBoolUniform(GLuint program, char *name, bool value)
{
    GLint uniform_location = glGetUniformLocation(program, name);
    glUniform1i(uniform_location, (int)value);
}

void SetIntUniform(GLuint program, char *name, int value)
{
    GLint uniform_location = glGetUniformLocation(program, name);
    glUniform1i(uniform_location, value);
}

void SetFloatUniform(GLuint program, char *name, float value)
{
    GLint uniform_location = glGetUniformLocation(program, name);
    glUniform1f(uniform_location, value);
}

void SetMat4Uniform(GLuint program, char *name, mat4 *matrix)
{
    GLint uniform_location = glGetUniformLocation(program, name);
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, &matrix->elements[0][0]);
}

struct EntityProgram
{
    GLuint program_handle;
    GLuint vao_handle;
    GLuint vbo_handle;
};

EntityProgram CreateEntityProgram(const char *vertex_shader_source,
                                  const char *fragment_shader_source)
{
    GLuint vao_handle, vbo_handle;
    glGenVertexArrays(1, &vao_handle);
    glGenBuffers(1, &vbo_handle);
    glBindVertexArray(vao_handle);
    
    float quad_vertices[] =
    {
        // Position, UVs
        0.0f, 0.0f, 0.0f, 0.0f, // lower left
        1.0f, 0.0f, 1.0f, 0.0f,  // rower right
        1.0f, 1.0f, 1.0f, 1.0f,   // upper right
        
        0.0f, 0.0f, 0.0f, 0.0f, // lower right
        1.0f, 1.0f, 1.0f, 1.0f,   // upper right
        0.0f, 1.0f, 0.0f, 1.0f   // upper left
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    GLuint program_handle = CreateProgram(vertex_shader_source, fragment_shader_source);
    
     EntityProgram result = {0};
    result.vao_handle = vao_handle;
    result.vbo_handle = vbo_handle;
    result.program_handle = program_handle;
    return result;
}
