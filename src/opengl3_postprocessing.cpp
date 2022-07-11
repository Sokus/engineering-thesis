/* Lazily creates an empty VAO
   (we use an empty VAO for postprocessing because
   in this case model is hardcoded into the shader)
*/
void bind_dummy_vao()
{
    static GLuint vao = 0;
    if(!vao) glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

/* Identity postprocessing program. Does nothing.

   Might be useful for debugging or when used with blending.
*/
LazyProgram identity_program(
    "./res/shaders/postprocessing.vs", 
    "./res/shaders/identity.fs"
);

/* Postprocessing program which applies a 1D convolution filter
   over the source texture.
*/
LazyProgram convolution_1d_program(
    "./res/shaders/postprocessing.vs", 
    "./res/shaders/convolution_1d.fs"
);

LazyProgram extract_bright_fragments_program(
    "./res/shaders/postprocessing.vs",
    "./res/shaders/extract_bright_fragments.fs"
);

// Allocates an empty texture with specified dimensions and internal format
GLuint alloc_texture(int width, int height, GLenum internal_format)
{
    GLuint texture_handle;
    glGenTextures(1, &texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture_handle);
    glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, width, height);
    return texture_handle;
}

// All framebuffers (and their textures) used by the application
struct Framebuffers
{
    int fbo_width, fbo_height;

    // Light map
    GLuint light_fbo;
    GLuint light_map;

    // Main fbo for drawing in-game entities
    GLuint draw_fbo;
    GLuint draw_texture;

    // Fbo used for computing the bloom effect
    GLuint bloom_fbo;
    GLuint bloom_texture;

    // Buffer fbo (e.g. for ping pong blur)
    GLuint buf_fbo;
    GLuint buf_texture;

    void init(int width, int height)
    {
        fbo_width = width;
        fbo_height = height;

        glGenFramebuffers(1, &light_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, light_fbo);
        light_map = alloc_texture(width, height, GL_RGB16F);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, light_map, 0);

        glGenFramebuffers(1, &draw_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, draw_fbo);
        draw_texture = alloc_texture(width, height, GL_RGBA16F);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, draw_texture, 0);

        glGenFramebuffers(1, &bloom_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, bloom_fbo);
        bloom_texture = alloc_texture(width, height, GL_RGBA16F);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloom_texture, 0);

        glGenFramebuffers(1, &buf_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, buf_fbo);
        buf_texture = alloc_texture(width, height, GL_RGBA16F);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buf_texture, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }  

    void dispose()
    {
        glDeleteFramebuffers(1, &buf_fbo);
        glDeleteTextures(1, &buf_texture);

        glDeleteFramebuffers(1, &bloom_fbo);
        glDeleteTextures(1, &bloom_texture);

        glDeleteFramebuffers(1, &draw_fbo);
        glDeleteTextures(1, &draw_texture);

        glDeleteFramebuffers(1, &light_fbo);
        glDeleteTextures(1, &light_map);
    }
};

// Normalizes a 1d convolution filter kernel so that all elements sum to 1
void normalize_kernel_1d(float *kernel, int length)
{
    float sum = 0;
    for(int i=0; i<length; ++i)
        sum += kernel[i];
    for(int i=0; i<length; ++i)
        kernel[i] /= sum;
}

// Creates a 1D gaussian blur kernel
void init_gaussian_blur_kernel_1d(float *kernel, int length, float variance)
{
    float offset = (-length / 2.0f) + 0.5f;
    for(int i=0; i<length; ++i)
    {
        float x = i + offset;
        kernel[i] = EXPF(-x*x/(2*variance))/SQRTF(2*PI32*variance);
    }
    normalize_kernel_1d(kernel, length);
}

// Creates a 1D box blur kernel
void init_box_blur_kernel_1d(float *kernel, int length)
{
    for(int i=0; i<length; ++i)
        kernel[i] = 1.0f / length;
}

/* Runs multiple passes of horizontal & vertical blur on texture tex
   attached to framebuffer fbo. The results are saved back to the texture tex.

   buf_tex is a buffer texture used by the algorithm. It must be attached
   to framebuffer but_fbo and must have the same or larger dimesions than tex.
*/
void ping_pong_blur(
    float *kernel, int length, int no_passes,
    GLuint fbo, GLuint tex,
    GLuint buf_fbo, GLuint buf_tex
) {
    // Get viewport information (x, y, width, height)
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float xstep = 1.0f / viewport[2];
    float ystep = 1.0f / viewport[3];

    // Initialize shader program
    GLuint program = convolution_1d_program;
    glUseProgram(program);
    SetFloatArrayUniform(program, "kernel", kernel, length);
    SetIntUniform(program, "kernel_length", length);

    // Bind textures to texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, buf_tex);

    glDisable(GL_BLEND);
    glClearColor(0,0,0,0);
    bind_dummy_vao();

    /* Each pass:
     * 1. Render contents of fbo to buf_fbo while applying vertical blur
     * 2. Render contents of buf_fbo back to fbo while applying horizontal blur
     */
    for(int pass = 0; pass < no_passes; ++pass)
    {
        for(int subpass = 0; subpass <= 1; ++subpass)  
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, subpass == 0 ? buf_fbo : fbo);
            glClear(GL_COLOR_BUFFER_BIT);
            SetIntUniform(program, "tex", subpass == 0 ? 0 : 1);
            SetVec2Uniform(program, "sampling_step", Vec2(subpass*xstep, (1-subpass)*ystep));
            glDrawArrays(GL_TRIANGLES, 0, 6);
        } 
    }
    glEnable(GL_BLEND);
}