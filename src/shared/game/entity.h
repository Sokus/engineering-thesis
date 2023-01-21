#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include "stdint.h"
#include "constans/constans.h"
#include <graphics/drawqueue.h>

struct BitStream;

namespace Game {

    struct Input;

    enum EntityType
    {
        ENTITY_TYPE_NONE,
        ENTITY_TYPE_PLAYER,
        ENTITY_TYPE_TILE,
        ENTITY_TYPE_INTERACTIVE,
        ENTITY_TYPE_MOVING_TILE,
        ENTITY_TYPE_COLLECTIBLE,
        ENTITY_TYPE_DAMAGING_TILE,
        ENTITY_TYPE_DESTRUCTIBLE_TILE,
        ENTITY_TYPE_CHECKPOINT,
        ENTITY_TYPE_ENEMY,
        ENTITY_TYPE_BULLET,
        ENTITY_TYPE_SHOCKWAVE,
        ENTITY_TYPE_EXIT,

        ENTITY_TYPE_COUNT, // ALWAYS LAST
    };
    enum EntityVariant {
        ENEMY_MOVE = 1,
        ENEMY_SHOOT_TOP = 2,
        ENEMY_SHOOT_SIDE = 4,
        ENEMY_FOLLOW = 8,
        ENEMY_JUMP = 16
    };

    void LoadEntityTextures();
    Texture GetEntityTexture(EntityType type);

    enum PlayerType
    {
        PLAYER_TYPE_ROUGE,
        PLAYER_TYPE_SNIPER,
        PLAYER_TYPE_HEALER,
        PLAYER_TYPE_WARRIOR,
        // ----------------
        PLAYER_TYPE_COUNT,
    };

    struct Entity
    {
        static const int max_base_health = 10'000;

        // NOTE: Only data that is absolutely necessary
        // to represent the game on the client side is serialized.
        // The client will not try to run the simulation himself thus
        // things like velocity, time remaining in animation frame or
        // state that does not affect visual appearance are not serialized.

        // SERIALIZED
        EntityType type;
        uint16_t revision;
        int owner;
        unsigned int current_frame;
        Vector2 size;
        Vector2 position;

        // NOT SERIALIZED
        int32_t variant;
        int entity_group; // used to connect tiles with interactibles
        Vector2 endpoints[2]; // ENTITY_TYPE_MOVING_TILE
        bool target;
        unsigned int num_frames;
        float frame_time;
        float max_frame_time;
        float time_until_state_change_allowed;
        float shot_cooldown;
        int health;
        int base_health; // move it out?
        float move_speed;
        float base_speed; // move it out?
        int damage;

        int money_count; // move it out when implementing the server
        bool on_ground;
        float jump_height; // move it out?
        int facing;
        bool ability_reset;
        float dash_cooldown;

        bool collideLeft, collideRight, collideTop;

        // TODO: Decide what to do with everything below this line
        // most of the things are either:
        // - weidly phrased/used
        // - unnecessary/derived from what is above
        // - do not belong to entity structure and should be held outside

        Vector2 move_direction;
        Vector2 velocity;

        bool collidable;
        bool active;

        void Update(float dt);
        void UpdateMovingTile(float dt);
        void setMoveSpeed(Input* input);
        void Draw(DrawQueue &dq) const;
        bool collidesWith(Entity ent);
        void calculateCollisionSide(Entity ent);
        void correctPositions(Entity ent,Vector2 velocity, Vector2 ent_vel,float dt);
        void correctEnemyPositions(Entity ent, Vector2 velocity, Vector2 ent_vel, float dt);
        void stalkPlayer(Entity ent,float dt);
        void Control(Input* input,float dt);
        void MoveX(float dt);
        void MoveY(float dt);
        bool reachedEndpoint(Vector2 target, float dt);
        void Despawn();
        Vector2 GetCenter() const;

        bool Serialize(BitStream *stream);

        /** Health normalized to range [0, 1]
         *  Intended for healthbars etc.
         */
        float relativeHealth() const;

        void drawHealthBar() const;
    };

    struct EntityReference
    {
        int index;
        uint16_t revision;
    };
}
#endif