#include "world.h"

#include <string.h>

namespace Game {

    void World::CalculateCollisions(Entity& player,float dt) {
        for (int entity_idx = 0; entity_idx < entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            if (entity->type == TILE) {
                if (player.collidesWithY(*entity, dt)) {
                    if (player.velocity.y > 0) {
                        player.position.y = entity->position.y - player.height * player.scale;
                        player.onGround = 1;
                        player.velocity.y = 0;
                    }
                    else if (player.velocity.y < 0) {
                        player.position.y = entity->position.y;
                        player.velocity.y = 0;
                    }
                }
                if (player.collidesWithX(*entity, dt)) {
                    if (player.velocity.x > 0) {
                        player.velocity.x = 0;
                        player.position.x = entity->position.x - player.width * player.scale;
                    }
                    if (player.velocity.x < 0) {
                        player.position.x = entity->position.x + player.width*player.scale;
                        player.velocity.x = 0;
                    }
                }
            }
            if (entity->type == INTERACTIVE) {
                if (player.collidesWithY(*entity, dt)) {
                    entity->active = 1;
                }
                else {
                    entity->active = 0;
                }
            }
        }
    }

    void World::Clear()
    {
        memset(entities, 0, sizeof(entities));
        level = {};
    }

    void World::Update(float dt)
    {
        level.Update(dt);
        
        for (int entity_idx = 0; entity_idx < entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            entity->Update(dt);
        }
    }

    void World::Draw()
    {
        level.DrawBackground({0,0});

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
            entity->jumpHeight = 15;
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
    Entity* World::CreateInteractive(float pos_x, float pos_y) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::INTERACTIVE;
            entity->position = glm::vec2(pos_x, pos_y);
            entity->move_speed = 0.0f;
            entity->max_animation_frame_time = 0.25f;
            entity->scale = 4;
        }
        return entity;
    }

    void World::SetLevel(const Level &level) {
        this->level = level;
    }

}