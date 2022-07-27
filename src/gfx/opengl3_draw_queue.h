#ifndef OPENGL3_DRAW_QUEUE
#define OPENGL3_DRAW_QUEUE

#include <vector>
#include <glm/glm.hpp>

#include <gfx/opengl3_lights.h>
#include <gfx/opengl3_shockwave.h>
#include <gfx/opengl3_entity.h>

class DrawQueue
{
    friend class Renderer;

    public:

    DrawQueue();
    
    void Clear();

    void SetViewportDimensions(const glm::ivec2 &value);
    const glm::ivec2 &GetViewportDimensions() const;

    void SetProjectionMatrix(const glm::mat4 &value);
    const glm::mat4 &GetProjectionMatrix() const;

    void SetViewMatrix(const glm::mat4 &value);
    const glm::mat4 &GetViewMatrix() const;

    void SetBackgroundColor(const glm::vec3 &value);
    const glm::vec3 &GetBackgroundColor() const;

    void SetAmbientLightColor(const glm::vec3 &value);
    const glm::vec3 &GetAmbientLightColor() const;

    void AddLight(const Light &light);
    void AddShockwave(const Shockwave &shockwave);
    void AddEntity(glm::vec2 position, const Texture &texture, int layer, bool flip);

    private:

    glm::ivec2 viewport_dimensions;
    glm::mat4 projection_matrix, view_matrix;

    glm::vec3 background_color, ambient_light_color;

    std::vector<Light> lights;
    std::vector<Shockwave> shockwaves;

    struct Entity {
        glm::vec2 position; 
        const Texture *texture;
        int layer;
        bool flip;
    };
    std::vector<Entity> entities;

};

#endif