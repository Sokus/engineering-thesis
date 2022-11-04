#include "game.h"
#include "input.h"
#include "macros.h"
#include "config.h"

#include "glm/glm.hpp"
#include "raylib.h"

namespace Game {

Texture2D character;

void Entity::Update(float dt)
{
    position += velocity * dt;

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
    if(move_direction.x != 0.0f) facing = (int)move_direction.x;

    velocity = move_direction * move_speed;
}

void Entity::Draw()
{
    if(type == PLAYER)
    {
        int scale = 4;
        int sprite_w = 16;
        int sprite_h = 24;
        Rectangle source = Rectangle{0.0f, 0.0f, 0.0f, (float)sprite_h};
        source.x = (float)(animation_frame % 4 * sprite_w);
        source.y = (ABSF(velocity.x) <= 0.001f ? 0.0f : (float)sprite_h);
        source.width = (float)(facing >= 0 ? sprite_w : -sprite_w);
        Rectangle dest = {
            position.x,
            position.y,
            (float)(scale * sprite_w),
            (float)(scale * sprite_h)
        };

        DrawTexturePro(character, source, dest, Vector2{0.0f, 0.0f}, 0.0f, Color{255, 255, 255, 255});
    }
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

void LoadTextures()
{
    character = LoadTexture(RESOURCE_PATH "/character.png");
}


} // namespace Game