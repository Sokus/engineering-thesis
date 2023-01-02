#ifndef ENTITY_H
#define ENTITY_H

#include "input.h"
#include "glm/glm.hpp"
#include <raylib.h>
#include <vector>
#include <math/rframe.h>
namespace Game {



    enum EntityType
    {
        NONE,
        PLAYER,
        TILE,
        INTERACTIVE,
        MOVING_TILE,
        COLLECTIBLE,
        DAMAGING_TILE,
        BULLET
    };
    

    struct Entity
    {
        // SHARED STATE
        EntityType type;
        Texture2D texture;

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

        float health;
        float move_speed;
        int stateChange;
        bool frameChange;
        int moneyCount;

        // MOVING DATA
        bool moving;
        glm::vec2 border[2];

        //BULLET DATA
        float damage = 0;
        float lifetime = 0;
        
        float maxLifetime = 1.5;
        float ln1MinusDragCoefficient = 0;
        /// Constant vertical acceleration applied to the bullet.
        float gravity;
        /// Visible width and height of the bullet's sprite.
        glm::vec2 visibleSize;
        glm::ivec2 animationFrames;
        /// Length of 1 full animation cycle, in seconds.
        float animationLength;
        std::vector<float> sizeKeyframes;
        std::vector<float> alphaKeyframes;
        /** drag coefficient = how much velocity is lost per second.
         *
         * e.g. 0.5 would cause the projectile to loose half of its velocity each second.
         */
        void SetDrag(float dragCoefficient);

        Game::ReferenceFrame referenceFrame;



        // CLIENT STATE

        // TODO: more robust animation system
        unsigned int animation_frame;
        float animation_frame_time;
        float max_animation_frame_time;

        void Update(float dt);
        void Draw();
        bool collidesWithX(Entity ent,float dt);
        bool collidesWithY(Entity ent, float dt);
        void Control(Input* input,float dt);
        bool inBorder(float dt);
    };
}
#endif