#include "world.h"
namespace Game {
    void World::CalculateCollisions(Entity& player) {
        for (int entity_idx = 0; entity_idx < entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            if (entity->type == TILE) {
                if (player.collidesWith(*entity)) {
                    player.velocity.x = 0.0f;
                    player.velocity.y = 0.0f;
                    printf("-1\n");
                }
                else {
                    printf("-0\n");
                }
            }
        }
    }

    void World::Update(float dt)
    {
        for (int entity_idx = 0; entity_idx < entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            entity->Update(dt);
        }
    }

    void World::Draw()
    {
        for (int entity_idx = 0; entity_idx < entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            entity->Draw();
        }
    }

    Entity* World::GetNewEntity()
    {
        Entity* result = nullptr;
        for (int entity_idx = 0; entity_idx < entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            if (entity->type == EntityType::NONE)
            {
                memset(entity, 0x00, sizeof(Entity));
                result = entity;
                break;
            }
        }
        return result;
    }

    Entity* World::CreatePlayer(float pos_x, float pos_y)
    {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::PLAYER;
            entity->position = glm::vec2(pos_x, pos_y);
            entity->move_speed = 100.0f;
            entity->max_animation_frame_time = 0.25f;
            entity->scale = 4;
        }
        return entity;
    }
    Entity* World::CreateTile(float pos_x, float pos_y) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::TILE;
            entity->position = glm::vec2(pos_x, pos_y);
            entity->move_speed = 0.0f;
            entity->max_animation_frame_time = 0.25f;
            entity->scale = 4;
        }
        return entity;
    }

}