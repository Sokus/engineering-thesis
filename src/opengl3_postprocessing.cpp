GLuint alloc_texture(int width, int height, GLenum internal_format)
{
    GLuint texture_handle;
    glGenTextures(1, &texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture_handle);
    glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, width, height);
    return texture_handle;
}

struct Framebuffers
{
    int fbo_width, fbo_height;

    GLuint light_fbo;
    GLuint light_map;

    GLuint draw_fbo;
    GLuint draw_texture;

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

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }  

    void dispose()
    {
        glDeleteFramebuffers(1, &draw_fbo);
        glDeleteTextures(1, &draw_texture);

        glDeleteFramebuffers(1, &light_fbo);
        glDeleteTextures(1, &light_map);
    }
};