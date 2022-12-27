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
        void Update(Input *input,float dt);
        void Draw();
        void CalculateCollisions(Entity& player,Input * input,float dt);
        void UpdateActiveTiles(float dt, int connectionGroup);


        Entity* GetNewEntity();
        Entity* CreatePlayer(float pos_x, float pos_y);
        Entity* CreateTile(float pos_x, float pos_y, int conGroup);
        Entity* CreateMovingTile(float pos_x, float pos_y, int conGroup, glm::vec2 moveDirection, glm::vec2 border[4]);
        Entity* CreateInteractive(float pos_x, float pos_y, int conGroup);
        void CreateBullet(const Bullet &bullet);

        void SetLevel(const Level &level);
    };

}

#endif