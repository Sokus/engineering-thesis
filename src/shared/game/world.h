#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include "entity.h"
#include "game/parallax.h"

struct BitStream;

namespace Game {
    const int max_entity_count = 1024;

    struct AddEntityResult
    {
        EntityReference reference;
        Entity *entity;
    };

    struct World
    {
        static const float gravity;
        static const float gravity_speed;
        Entity entities[max_entity_count] = {};
        Vector2 spawnpoint;
        ParallaxBackground parallax_background;

        bool initialised;
        bool finished;

        void Clear();
        void Update(Input *inputs, int num_inputs, float dt);

        void Draw();
        void DrawHealthBars() const;
        void ControlEnemy(Entity& enemy, float dt);
        void MovePlayer(Entity& player,Input* input, float dt);
        void MoveEnemy(Entity& player, float dt);
        void CheckPlayerShot(Entity& player, Input* input, float dt);
        void CalculateCollisions(Entity& player, float dt, bool dim);
        void CalculateEnemyCollisions(Entity& player, Vector2 velocity, float dt, bool dim);
        void CheckStates(Entity& player, Vector2 velocity, Input* input, float dt);
        void UpdateActiveTiles(float dt, int connectionGroup);

        EntityReference GetFreeEntityReference();
        EntityReference GetOwnedEntityReference(EntityType type, int owner, int start_index);
        bool EntityReferenceIsValid(EntityReference reference);
        Entity *GetEntityByReference(EntityReference reference);
        AddEntityResult AddEntity(EntityType type, int owner, float x, float y, float w, float h);
        void FreeEntity(Entity *entity);
        void FreeOwnedEntities(int owner);

        AddEntityResult CreatePlayer(int owner, float pos_x, float pos_y, Game::PlayerType playertype);
        AddEntityResult CreateTile(float pos_x, float pos_y,float width,float height, int conGroup,bool enabled);
        Entity* CreateMovingTile(float pos_x, float pos_y, float width, float height, int conGroup, Vector2 moveDirection, Vector2 endpoint,bool enabled);
        Entity* CreateInteractive(float pos_x, float pos_y, float width, float height, int conGroup);
        Entity* CreateCollectible(float pos_x, float pos_y, float width, float height, int conGroup);
        Entity* CreateDamagingTile(float pos_x, float pos_y, float width, float height, int conGroup);
        Entity* CreateDestroyTile(float pos_x, float pos_y, float width, float height, int conGroup);
        Entity* CreateCheckpoint(float pos_x, float pos_y, float width, float height);
        Entity* CreateExit(float pos_x, float pos_y, float width, float height);
        Entity* CreateEnemy(float pos_x, float pos_y, float width, float height,int32_t property,int health);
        Entity* CreateBullet(float pos_x, float pos_y, float width, float height,float vel_x,float vel_y,int owner);
        void LoadTextures();
        void hitObstacles(Entity &bullet);
    };
}

#endif