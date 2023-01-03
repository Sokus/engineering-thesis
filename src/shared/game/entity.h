#ifndef ENTITY_H
#define ENTITY_H

#include "glm/glm.hpp"
#include "math/rframe.h"
#include "raylib.h"

#include <vector>

namespace Game {

    struct Input;


    enum EntityType
    {
        NONE,
        PLAYER,
        TILE,
        INTERACTIVE,
        MOVING_TILE,
        COLLECTIBLE,
        DAMAGING_TILE,
        BULLET,
        DESTROY_TILE,
        CHECKPOINT,
        EXIT
    };
    enum PlayerType
    {
        ROUGE,
        SNIPER,
        HEALER,
        WARRIOR
    };

    struct BulletData {
        //BULLET DATA
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
    };

    struct PlayerData {
        int moneyCount;
        bool onGround;
        int jumpHeight;
        int facing;
        bool ability_reset;
    };


    struct Entity
    {
        int width;
        int height;
        int scale;

        // SHARED STATE

        glm::vec2 move_direction= glm::vec2(0.0f, 0.0f);

        float move_speed;
        float base_speed;
        int base_health;
        int health;

        bool frameChange;
        int connectionGroup;
        int stateChange;
        float damage;

        //STATE DATA
        bool collidable;
        bool enabled;
        bool visible;


        // MOVING DATA
        glm::vec2 border[2];

        EntityType type;
        Texture2D texture;
        Game::PlayerData playerData;
        Game::BulletData bulletData;
        Game::ReferenceFrame rF;


        // CLIENT STATE

        // TODO: more robust animation system
        unsigned int animation_frame;
        float animation_frame_time;
        float max_animation_frame_time;

        void Update(float dt);
        void setMoveSpeed(Input* input);
        void Draw();
        bool collidesWith(Entity ent);
        void Control(Input* input,float dt);
        void MoveX(float dt);
        void MoveY(float dt);
        bool inBorder(float dt);
    };
}
#endif