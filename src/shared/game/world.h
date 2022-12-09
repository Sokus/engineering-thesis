#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "entity.h"
#include "bullet.h"
#include "level.h"

namespace Game {

    class World
    {
    private:
        static float gravity;
        static float gravity_speed;
        static const int entity_count = 256;
        Entity entities[entity_count] = {};
        std::vector<Bullet> bullets;
        Level level;

    public:
        void Clear();
        void Update(float dt);
        void Draw();
        void CalculateCollisions(Entity& player,float dt);

        Entity* GetNewEntity();
        Entity* CreatePlayer(float pos_x, float pos_y);
        Entity* CreateTile(float pos_x, float pos_y);
        Entity* CreateInteractive(float pos_x, float pos_y);
        void CreateBullet(const Bullet &bullet);

        void SetLevel(const Level &level);
    };

}

#endif