#ifndef RENDERER_H
#define RENDERER_H

#include <gfx/opengl3_draw_queue.h>
#include <gfx/opengl3_postprocessing.h>

class Renderer
{
    public:

    Renderer();
    Renderer(const Renderer &) = delete;
    ~Renderer();

    void render(const DrawQueue &);

    private:

    EntityRenderer entity_renderer;
    LightRenderer light_renderer;
    ShockwaveRenderer shockwave_renderer;

    Framebuffers framebuffers;
};

#endif