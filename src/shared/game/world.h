#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include "entity.h"
#include "level/content.h"

namespace Game {

    struct AddEntityResult
    {
        EntityReference reference;
        Entity *entity;
    };

    class World
    {
    private:
        static float gravity;
        static float gravity_speed;
        static const int max_entity_count = 1024;
        Entity entities[max_entity_count] = {};

    public:
        bool initialised;
        Level level;
        int entity_count = 1;

        void Clear();
        void Update(Input *input, float dt);
        void Draw();
        void MoveEnemy(Entity& enemy, float dt);
        void MovePlayer(Entity& player,Input* input, float dt);
        void CheckPlayerShot(Entity& player, Input* input, float dt);
        void CalculateCollisions(Entity& player, Vector2 velocity, Input* input, float dt, bool dim);
        void UpdateActiveTiles(float dt, int connectionGroup);

        EntityReference GetFreeEntityReference();
        Entity *GetEntityByReference(EntityReference reference);
        AddEntityResult World::AddEntity(EntityType type, float x, float y, float w, float h, Texture2D texture);
        void World::FreeEntity(Entity *entity);

        AddEntityResult CreatePlayer(float pos_x, float pos_y,Texture2D texture, Game::PlayerType playertype);
        AddEntityResult CreateTile(float pos_x, float pos_y,float width,float height, int conGroup, Texture2D texture);
        Entity* CreateMovingTile(float pos_x, float pos_y, float width, float height, int conGroup, Vector2 moveDirection, Vector2 endpoint, Texture2D texture);
        Entity* CreateInteractive(float pos_x, float pos_y, float width, float height, int conGroup, Texture2D texture);
        Entity* CreateCollectible(float pos_x, float pos_y, float width, float height, int conGroup, Texture2D texture);
        Entity* CreateDamagingTile(float pos_x, float pos_y, float width, float height, int conGroup, Texture2D texture);
        Entity* CreateDestroyTile(float pos_x, float pos_y, float width, float height, int conGroup, Texture2D texture);
        Entity* CreateCheckpoint(float pos_x, float pos_y, float width, float height, Texture2D texture);
        Entity* CreateExit(float pos_x, float pos_y, float width, float height, Texture2D texture);
        Entity* CreateEnemy(float pos_x, float pos_y, float width, float height, Texture2D texture);
        Entity* World::CreateBullet(float pos_x, float pos_y, float width, float height,float vel_x,float vel_y, Texture2D texture);
        void LoadTextures();
        void SetLevel(Level *level);
        void ClearLevel();
        void hitObstacles(Entity &bullet);
    };
}

#endif