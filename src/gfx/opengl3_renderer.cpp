#include "opengl3_renderer.h"
#include "base/base.h"
#include "os/sdl2/sdl2_platform.h"

Renderer::Renderer()
{
    framebuffers.dispose();
    entity_renderer.init();
    light_renderer.init();
    shockwave_renderer.init();
}

Renderer::~Renderer()
{
    shockwave_renderer.dispose();
    light_renderer.dispose();
    entity_renderer.dispose();

    int window_width, window_height;
    OS::GetWindowDimensions(&window_width, &window_height);
    framebuffers.init(window_width, window_height);
}

void Renderer::render(const DrawQueue &dq)
{
    // Ensure framebuffers have same dimensions as the viewport
    int viewport_width = dq.viewport_dimensions.x,
        viewport_height = dq.viewport_dimensions.y;
    if(viewport_width != framebuffers.fbo_width ||
        viewport_height != framebuffers.fbo_height)
    {
        framebuffers.dispose();
        framebuffers.init(viewport_width, viewport_height);
    }

    glm::mat4 view_projection_matrix = dq.projection_matrix * dq.view_matrix;

    glViewport(0, 0, viewport_width, viewport_height);



    // Render lights into light map
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.light_fbo);

    glClearColor(dq.ambient_light_color.r, dq.ambient_light_color.g, dq.ambient_light_color.b, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glBlendEquation(GL_MAX);
    light_renderer.render(view_projection_matrix, dq.lights.data(), dq.lights.size(), 1);
    glBlendEquation(GL_FUNC_ADD);
    
    // Render lights & entities into draw_fbo:
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.draw_fbo);

    glClearColor(dq.background_color.r, dq.background_color.g, dq.background_color.b, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    light_renderer.render(view_projection_matrix, dq.lights.data(), dq.lights.size(), 0.15f);
    entity_renderer.render(view_projection_matrix, dq.entities.data(), dq.entities.size(), framebuffers.light_map);

    
    // Extract bright fragments from draw_fbo into bloom_fbo
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.bloom_fbo);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(extract_bright_fragments_program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuffers.draw_texture);
    SetIntUniform(extract_bright_fragments_program, "tex", 0);

    bind_dummy_vao();
    glDrawArrays(GL_TRIANGLES, 0, 6);



    // Apply blur to bloom_fbo
    float kernel[15], variance = 5;
    int no_passes = 5;
    init_gaussian_blur_kernel_1d(kernel, ARRAY_SIZE(kernel), variance);
    ping_pong_blur(kernel, ARRAY_SIZE(kernel), no_passes,
                    framebuffers.bloom_fbo, framebuffers.bloom_texture,
                    framebuffers.buf_fbo, framebuffers.buf_texture);


    
    // Apply bloom effect to draw_fbo
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.draw_fbo);
    glUseProgram(identity_program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuffers.bloom_texture);
    SetIntUniform(identity_program, "tex", 0);
    bind_dummy_vao();
    glBlendFunc(GL_ONE, GL_ONE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    // Render shockwaves
    shockwave_renderer.render(
        view_projection_matrix, 
        framebuffers.draw_texture, 
        dq.shockwaves.data(), 
        dq.shockwaves.size()
    );



    // Blit (copy) postprocessing results into default framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers.draw_fbo);
    glBlitFramebuffer(0, 0, viewport_width, viewport_height, 0, 0, viewport_width, viewport_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}