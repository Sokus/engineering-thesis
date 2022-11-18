#ifndef WORLD_H
#define WORLD_H

#include "game.h"
#include "entity.h"
namespace Game {

    class World
    {
    private:
        static const int entity_count = 256;
        Entity entities[entity_count] = {};

    public:
        void Update(float dt);
        void Draw();
        void CalculateCollisions(Entity& player);

        Entity* GetNewEntity();
        Entity* CreatePlayer(float pos_x, float pos_y);
        Entity* CreateTile(float pos_x, float pos_y);
    };
}

#endif