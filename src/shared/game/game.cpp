#include "game.h"
#include "input.h"
#include "macros.h"
#include "config.h"

#include "glm/glm.hpp"

namespace Game {


void Entity::Update(float dt)
{
    position += velocity * dt;
    if(velocity.x > 0.0f) facing = 1;
    if(velocity.x < 0.0f) facing = -1;

    if(type == PLAYER)
    {
        if(animation_frame_time >= max_animation_frame_time)
        {
            animation_frame++;
            animation_frame_time = 0.0f;
        }

        animation_frame_time += dt;
    }
}

void Entity::Control(Input *input)
{
    ASSERT(type == PLAYER);

    glm::vec2 move_direction = glm::vec2(0.0f, 0.0f);
    if(input->move[Input::Direction::LEFT]) move_direction.x -= 1.0f;
    if(input->move[Input::Direction::RIGHT]) move_direction.x += 1.0f;

    velocity = move_direction * move_speed;
}

void World::Update(float dt)
{
    for(int entity_idx = 0; entity_idx < entity_count; entity_idx++)
    {
        Entity *entity = entities + entity_idx;
        entity->Update(dt);
    }
}

void World::Draw()
{
    for(int entity_idx = 0; entity_idx < entity_count; entity_idx++)
    {
        Entity *entity = entities + entity_idx;
        entity->Draw();
    }
}

Entity *World::GetNewEntity()
{
    Entity *result = nullptr;
    for(int entity_idx = 0; entity_idx < entity_count; entity_idx++)
    {
        Entity *entity = entities + entity_idx;
        if(entity->type == EntityType::NONE)
        {
            memset(entity, 0x00, sizeof(Entity));
            result = entity;
            break;
        }
    }
    return result;
}

Entity *World::CreatePlayer(float pos_x, float pos_y)
{
    Entity *entity = nullptr;
    if(entity = GetNewEntity())
    {
        entity->type = EntityType::PLAYER;
        entity->position = glm::vec2(pos_x, pos_y);
        entity->move_speed = 100.0f;
        entity->max_animation_frame_time = 0.25f;
    }
    return entity;
}

} // namespace Game