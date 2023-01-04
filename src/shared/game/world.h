#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include "entity.h"
#include "level/content.h"

namespace Game {

    extern Level ActualLevel;
    extern PlayerType ActualPlayer;

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
        Level level;


    public:
        int entity_count = 1;
        void Clear();
        void Update(Input *input,float dt);
        void Draw();
        void CalculateCollisions(Entity& player, Vector2 velocity,Input * input, float dt, bool dim);
        void UpdateActiveTiles(float dt, int connectionGroup);

        EntityReference GetFreeEntityReference();
        Entity *GetEntityByReference(EntityReference reference);
        AddEntityResult World::AddEntity(EntityType type, float x, float y, float w, float h, Texture2D texture);
        void World::FreeEntity(Entity *entity);

        AddEntityResult CreatePlayer(float pos_x, float pos_y,Texture2D texture, Game::PlayerType playertype);
        AddEntityResult CreateTile(float pos_x, float pos_y, int conGroup, Texture2D texture);
        Entity* CreateMovingTile(float pos_x, float pos_y, int conGroup, Vector2 moveDirection, Vector2 border[2], Texture2D texture);
        Entity* CreateInteractive(float pos_x, float pos_y, int conGroup, Texture2D texture);
        Entity* CreateCollectible(float pos_x, float pos_y, int conGroup, Texture2D texture);
        Entity* CreateDamagingTile(float pos_x, float pos_y, int conGroup, Texture2D texture);
        Entity* CreateDestroyTile(float pos_x, float pos_y, int conGroup, Texture2D texture);
        Entity* CreateCheckpoint(float pos_x, float pos_y, Texture2D texture);
        Entity* CreateExit(float pos_x, float pos_y, Texture2D texture);
        void LoadTextures();
        void SetLevel(Level &level);
        void ClearLevel();
        void hitObstacles(Entity& bullet);
    };

}

#endif