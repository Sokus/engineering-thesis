struct Light
{
    vec2 position;
    vec3 color;
    /* Formula for light intensity:
       max(0, color / (k_quadratic*d*d + k_linear*d + k_const) - k_bias)
       where d=distance from center
    */
    union
    {
        vec4 attenuation;
        struct {
            float k_quadratic, k_linear, k_const, k_bias;
        };
    };
};

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
int solve_quadratic(float a, float b, float c, float *x1, float *x2)
{
    float delta = b*b - 4*a*c;

    if(ABS(a) < 1e-6f)
    {
        if(ABS(b) > 1e-6f)
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

class LightRenderer
{
    private:

    struct Vertex 
    {
        vec2 position;
        Light light;
    };

    GLuint vbo, vao, shader_program;
    GLsizei vbo_capacity;

    public:

    void init() 
    {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // note: we need to actually create the buffer before calling glVertexAttribPointer
        vbo_capacity = 128;
        glBufferData(GL_ARRAY_BUFFER, vbo_capacity * sizeof(Vertex), nullptr, GL_STREAM_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Vertex), (const void *) offsetof(Vertex, position));
        glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (const void *) (offsetof(Vertex, light) + offsetof(Light, position)));
        glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (const void *) (offsetof(Vertex, light) + offsetof(Light, color)));
        glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(Vertex), (const void *) (offsetof(Vertex, light) + offsetof(Light, attenuation)));

        for(int i=0; i<4; ++i) glEnableVertexAttribArray(i);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        shader_program = CreateProgramFromFiles("./res/shaders/omni.vs", "./res/shaders/omni.fs");
    }

    void dispose() 
    {
        glDeleteProgram(shader_program);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    void render(const mat4 &view_projection, const Light *lights, int count) 
    {
        constexpr int vertices_per_light = 6;
        GLsizei no_vertices = vertices_per_light * count;

        // Reallocate the VBO if its too small
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        if(vbo_capacity < no_vertices) 
        {
            vbo_capacity = MAX(no_vertices, vbo_capacity * 3/2);
            glBufferData(GL_ARRAY_BUFFER, vbo_capacity*sizeof(Vertex), nullptr, GL_STREAM_DRAW);
        }

        // Fill the VBO
        Vertex *buffer = (Vertex *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

        // We will render a quad for each light
        // The size of the quad will be determined by the maximum range of the light
        // (how far the light can reach)
        vec2 position_lookup[vertices_per_light] = {
            Vec2(-1,-1), Vec2(1,-1), Vec2(-1,1),
            Vec2(-1,1), Vec2(1,-1), Vec2(1,1)
        };

        int no_vertices_written = 0;
        for(int i=0; i<count; ++i) 
        {
            Light light = lights[i];
            float brightest_component = MAX(MAX(light.color.r, light.color.g), light.color.b);
            float range;

            // Skip lights for which range can not be calculated
            if(!solve_quadratic(
                light.k_quadratic * light.k_bias,
                light.k_linear * light.k_bias,
                light.k_const * light.k_bias - brightest_component,
                nullptr, &range
            )) continue;

            // Skip lights with range <= 0
            if(range <= 0) 
                continue;

            for(int j=0; j<vertices_per_light; ++j)
            {
                vec2 vertex_pos = AddVec2(MultiplyVec2(position_lookup[j], Vec2(range, range)), light.position);
                buffer[no_vertices_written++] = {vertex_pos, light};
            }
        }

        glUnmapBuffer(GL_ARRAY_BUFFER);

        // Render the lights
        glBindVertexArray(vao);
        glUseProgram(shader_program);
        SetMat4Uniform(shader_program, "view_projection", &view_projection);
        glBlendFunc(GL_ONE, GL_ONE);
        glDrawArrays(GL_TRIANGLES, 0, no_vertices_written);

        //Cleanup
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindVertexArray(0);
        glUseProgram(0);
    }
};
