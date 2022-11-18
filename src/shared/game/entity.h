#ifndef ENTITY_H
#define ENTITY_H

#include "input.h"
#include "glm/glm.hpp"
namespace Game {
    enum EntityType
    {
        NONE,
        PLAYER,
        TILE
    };

    struct Entity
    {
        // SHARED STATE
        EntityType type;

        glm::vec2 position;
        glm::vec2 velocity;

        int facing;
        int width;
        int height;
        int scale;

        float move_speed;

        // CLIENT STATE

        // TODO: more robust animation system
        unsigned int animation_frame;
        float animation_frame_time;
        float max_animation_frame_time;

        void Update(float dt);
        void Draw();
        bool collidesWith(Entity ent);
        void Control(Input* input);
    };
}
#endif