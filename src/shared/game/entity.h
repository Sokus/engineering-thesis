#ifndef ENTITY_H
#define ENTITY_H

#include "input.h"
#include "glm/glm.hpp"
namespace Game {


    void LoadTextures();

    enum EntityType
    {
        NONE,
        PLAYER,
        TILE,
        INTERACTIVE,
        MOVING_TILE
    };

    struct Entity
    {
        // SHARED STATE
        EntityType type;

        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec2 move_direction= glm::vec2(0.0f, 0.0f);;

        int facing;
        int width;
        int height;
        int scale;
        bool onGround;
        int jumpHeight;
        bool active;
        int connectionGroup;

        // PLAYER DATA 

        float move_speed;
        int stateChange;
        bool frameChange;

        // MOVING DATA
        bool moving;
        glm::vec2 border[4];

        // CLIENT STATE

        // TODO: more robust animation system
        unsigned int animation_frame;
        float animation_frame_time;
        float max_animation_frame_time;

        void Update(float dt);
        void Draw();
        bool collidesWithX(Entity ent,float dt);
        bool collidesWithY(Entity ent, float dt);
        void Control(Input* input);
        bool inBorder(float dt);
    };
}
#endif