#include "world.h"
#include "input.h"

#include "raylib.h"
#include "raymath.h"

#include <string.h>

namespace Game {

    void World::MoveEnemy(Entity& enemy,float dt) {
        Vector2 ent_pos;
        for (int collide_idx = 0; collide_idx < entity_count; collide_idx++)
        {
            Entity* collideEntity = entities + collide_idx;
            if (collideEntity->type != ENTITY_TYPE_NONE && collideEntity->collidable) {
                ent_pos = Vector2Add(collideEntity->position ,(Vector2Scale(collideEntity->velocity,dt)));
                if (enemy.collidesWith(*collideEntity)) {
                    if (enemy.velocity.x > 0) {
                        enemy.velocity.x = enemy.velocity.x * -1;
                        enemy.position.x = ent_pos.x - enemy.size.x;
                    }
                    else if (enemy.velocity.x < 0) {
                        enemy.velocity.x = enemy.velocity.x * -1;
                        enemy.position.x = ent_pos.x + collideEntity->size.x;
                    }
                }
            }

        }
    }

    void World::CalculateCollisions(Entity& player, Vector2 velocity, Input* input, float dt,bool dim) {
        for (int collide_idx = 0; collide_idx < max_entity_count; collide_idx++)
        {
            Entity* collideEntity = entities + collide_idx; //TODO add reaction to squishing - dmg, teleport?
            if (collideEntity->type!= ENTITY_TYPE_NONE && collideEntity->collidable) {
                Vector2 ent_pos = {};
                ent_pos.x = collideEntity->position.x + collideEntity->velocity.x * dt;
                ent_pos.y = collideEntity->position.y + collideEntity->velocity.y * dt;

                Vector2 ent_vel = {};
                if (dim == 0) {
                    ent_vel.x = collideEntity->velocity.x;
                    ent_vel.y = 0;
                }
                else {
                    ent_vel.x = 0;
                    ent_vel.y = collideEntity->velocity.y;
                }
                if (player.collidesWith(*collideEntity)) {
                    //player.calculateCollisionSide(*collideEntity);
                    if (velocity.x > 0) {
                        player.velocity.x = 0;
                        player.position.x = ent_pos.x - player.size.x;
                    }
                    if (velocity.x < 0) {
                        player.velocity.x = 0;
                        player.position.x = ent_pos.x + collideEntity->size.x;
                    }
                    if (velocity.y < 0) {
                        player.position.y = collideEntity->position.y + collideEntity->size.y;
                        player.velocity.y = 0;
                        player.move_direction.y = 0;
                        if (ent_vel.y > 0) {
                            player.position.y = ent_pos.y + collideEntity->size.y;
                        }
                    }
                    if (velocity.y > 0) {
                        player.position.y = collideEntity->position.y - player.size.y;
                        player.on_ground = true;
                        player.ability_reset = true;
                        player.velocity.y = 0;
                        if (ent_vel.y < 0) {
                            float offset = ent_pos.y - collideEntity->position.y;
                            player.position.y = collideEntity->position.y - player.size.y + offset;
                        }
                    }
                    if (ent_vel.x > 0) {
                        player.position.x = collideEntity->position.x + player.size.x;
                        player.collideLeft = true;
                    }
                    if (ent_vel.x < 0) {
                        player.position.x = collideEntity->position.x - player.size.x;
                        player.collideRight = true;
                    }
                }
            }
            else if (collideEntity->type == ENTITY_TYPE_INTERACTIVE) {
                if (player.collidesWith(*collideEntity) && input->interact
                    && collideEntity->time_until_state_change_allowed <= 0.0f) {
                    collideEntity->active = !collideEntity->active;
                    UpdateActiveTiles(dt, collideEntity->entity_group);
                    collideEntity->time_until_state_change_allowed = 0.25f; // change if state is the same longer then 1/4 of sec
                }
            }
            else if (collideEntity->type == ENTITY_TYPE_COLLECTIBLE) {
                if (player.collidesWith(*collideEntity)) {
                    player.money_count++;
                    FreeEntity(collideEntity);
                }
            }
            else if (collideEntity->type == ENTITY_TYPE_DAMAGING_TILE) {
                if (player.collidesWith(*collideEntity)
                    && collideEntity->time_until_state_change_allowed <= 0.0f) {
                    player.health -= collideEntity->damage;
                    collideEntity->time_until_state_change_allowed = 1.5f; //hurts every 1.5 of sec
                }
            }
            else if (collideEntity->type == ENTITY_TYPE_CHECKPOINT) {
                if (player.collidesWith(*collideEntity) && !collideEntity->active) {
                    float pos_y = collideEntity->position.y + collideEntity->size.y - player.size.y;
                    collideEntity->active = true;
                    level.spawnpoint = Vector2{collideEntity->position.x, pos_y};
                }
            }
            else if (collideEntity->type == ENTITY_TYPE_EXIT) {
                if (player.collidesWith(*collideEntity)) {
                    level.finished = 1;
                }
            }
        }
    }

    void World::UpdateActiveTiles(float dt, int connGroup) {
        for (int entity_idx = 0; entity_idx < max_entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            if (entity->type == ENTITY_TYPE_TILE && entity->entity_group == connGroup) {
                entity->active = !entity->active;
                entity->collidable = !entity->collidable;
            }
        }
    }
    void World::hitObstacles(Entity &bullet) {
        for (int entity_idx = 0; entity_idx < max_entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            if (entity->type == ENTITY_TYPE_DESTRUCTIBLE_TILE && bullet.collidesWith(*entity) ) {
                entity->health -= bullet.damage;
                if (entity->health <= 0) {
                    entity->type = ENTITY_TYPE_NONE;
                }
                bullet.type = ENTITY_TYPE_NONE;
            }
        }
    }

    void World::Clear()
    {
        memset(entities, 0, sizeof(max_entity_count * sizeof(Entity)));
        level = {};
    }

    void World::Update(Input *input, float dt)
    {
        level.Update(dt);

        for (int entity_idx = 0; entity_idx < max_entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            entity->Update(dt);
            if (entity->type == ENTITY_TYPE_PLAYER) {
                if (entity->health <= 0) {
                    entity->position = level.spawnpoint; //Add some death animation
                    entity->health = entity->base_health;
                    break;
                }
                entity->Control(input,dt);
                entity->MoveX(dt);
                CalculateCollisions(*entity, Vector2{entity->velocity.x,0.0f}, input, dt,0);
                entity->on_ground = 0;
                entity->MoveY(dt);
                CalculateCollisions(*entity, Vector2{0.0f,entity->velocity.y}, input, dt,1);
                if ((entity->on_ground && entity->collideTop) || (entity->collideLeft && entity->collideRight)) {
                    entity->health = 0;
                }
                entity->collideLeft = 0;
                entity->collideRight = 0;
                entity->collideTop = 0;
            }
            else if (entity->type == ENTITY_TYPE_ENEMY) {
                MoveEnemy(*entity,dt);
            }

        }
    }

    void World::Draw()
    {
        level.DrawBackground({0,0});

        for (int entity_idx = 0; entity_idx < max_entity_count; entity_idx++)
        {
            Entity *entity = &entities[entity_idx];
            if (entity->type != ENTITY_TYPE_NONE)
                entity->Draw();
        }
    }

    EntityReference World::GetFreeEntityReference()
    {
        EntityReference reference = {};
        for (int i = 1; i < max_entity_count; i++)
        {
            if (entities[i].type == ENTITY_TYPE_NONE)
            {
                reference.index = i;
                reference.revision = entities[i].revision;
                break;
            }
        }
        return reference;
    }

    Entity *World::GetEntityByReference(EntityReference reference)
    {
        Entity *entity = nullptr;
        if (reference.index > 0 && reference.index < max_entity_count)
        {
            if (entities[reference.index].revision == reference.revision)
                entity = &entities[reference.index];
        }
        return entity;
    }

    AddEntityResult World::AddEntity(EntityType type, float x, float y, float w, float h, Texture2D texture)
    {
        AddEntityResult result = {};
        EntityReference reference = GetFreeEntityReference();
        Entity *entity = GetEntityByReference(reference);

        if (entity)
        {
            memset(entity, 0, sizeof(Entity));
            entity->type = type;
            entity->position = Vector2{x, y};
            entity->size.x = w;
            entity->size.y = h;
            entity->texture = texture;

            result.reference = reference;
            result.entity = entity;
        }

        return result;
    }

    void World::FreeEntity(Entity *entity)
    {
        if (entity)
        {
            if (entity->type != ENTITY_TYPE_NONE)
            {
                entity->type = ENTITY_TYPE_NONE;
                entity->revision++;
            }
        }
    }

    AddEntityResult World::CreatePlayer(float pos_x, float pos_y,Texture2D texture,Game::PlayerType playertype)
    {
        AddEntityResult result = AddEntity(ENTITY_TYPE_PLAYER, pos_x, pos_y, 16, 24, texture);
        if (result.entity)
        {
            Entity *entity = result.entity;
            entity->num_frames = 4;
            entity->max_frame_time = 0.25f;
            entity->collidable = 0;
            switch (playertype)
            {
            case PLAYER_TYPE_ROUGE:
                entity->base_health = 80;
                entity->base_speed = 55.0f;
                entity->jump_height = 10.0f;
                break;
            case PLAYER_TYPE_SNIPER:
                entity->base_health = 70;
                entity->base_speed = 50.0f;
                entity->jump_height = 3.0f;
                break;
            case PLAYER_TYPE_HEALER:
                entity->base_health = 100;
                entity->base_speed = 45.0f;
                entity->jump_height = 5.0f;
                break;
            case PLAYER_TYPE_WARRIOR:
                entity->base_health = 120;
                entity->base_speed = 37.5f;
                entity->jump_height = 2.25f;
                break;
            default: break;
            }
            entity->move_speed = entity->base_speed;
            entity->health = entity->base_health;
        }
        return result;
    }

    AddEntityResult World::CreateTile(float pos_x, float pos_y, float width,float height,int conGroup, Texture2D texture)
    {
        AddEntityResult result = AddEntity(ENTITY_TYPE_TILE, pos_x, pos_y, width, height, texture);
        if (result.entity)
        {
            Entity *entity = result.entity;
            entity->entity_group = conGroup;
            entity->collidable = true;
            entity->active = true;
        }
        return result;
    }

    Entity* World::CreateInteractive(float pos_x, float pos_y,float width,float height,int conGroup, Texture2D texture)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_INTERACTIVE, pos_x, pos_y, width,height, texture).entity)
        {
            entity->entity_group = conGroup;
        }
        return entity;
    }

    Entity* World::CreateMovingTile(float pos_x, float pos_y,float width,float height, int conGroup, Vector2 moveDirection, Vector2 endpoint, Texture2D texture)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_MOVING_TILE, pos_x, pos_y, width,height, texture).entity)
        {
            entity->move_direction.x = (pos_x - endpoint.x)*-1;
            entity->move_direction.y = (pos_y - endpoint.y)*-1;
            entity->move_speed = 1.0f;
            entity->entity_group = conGroup;
            entity->active = true;
            entity->endpoints[1] = Vector2{pos_x, pos_y};
            entity->endpoints[0] = Vector2{ endpoint.x, endpoint.y };
            entity->collidable = true;
        }
        return entity;
    }

    Entity* World::CreateCollectible(float pos_x, float pos_y, float width, float height, int conGroup, Texture2D texture)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_COLLECTIBLE, pos_x, pos_y, width, height, texture).entity)
        {
            entity->entity_group = conGroup;
            entity->active = true;
        }
        return entity;
    }

    Entity* World::CreateDamagingTile(float pos_x, float pos_y, float width, float height, int conGroup, Texture2D texture)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_DAMAGING_TILE, pos_x, pos_y, width,height, texture).entity)
        {
            entity->entity_group = conGroup;
            entity->active = true;
            entity->damage = 10;
        }
        return entity;
    }

    Entity* World::CreateDestroyTile(float pos_x, float pos_y, float width, float height, int conGroup, Texture2D texture)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_DESTRUCTIBLE_TILE, pos_x, pos_y, width,height, texture).entity)
        {
            entity->entity_group = conGroup;
            entity->active = true;
            entity->health = 15;
            entity->collidable = 1;
        }
        return entity;
    }

    Entity* World::CreateCheckpoint(float pos_x, float pos_y, float width, float height, Texture2D texture)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_CHECKPOINT, pos_x, pos_y, width,height, texture).entity)
        {

        }
        return entity;
    }

    Entity* World::CreateExit(float pos_x, float pos_y, float width, float height, Texture2D texture)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_EXIT, pos_x, pos_y, width,height, texture).entity)
        {

        }
        return entity;
    }
    Entity* World::CreateEnemy(float pos_x, float pos_y, float width, float height, Texture2D texture)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_ENEMY, pos_x, pos_y, width, height, texture).entity)
        {
            entity->velocity.x = 10.0f;
        }
        return entity;
    }


    void World::SetLevel(Level *level) {
        this->level = *level;
        this->level.LoadTextures();
        for (auto& tile : level->tiles) {
            this->CreateTile(tile.position.x, tile.position.y,tile.size.x,tile.size.y, tile.connGroup, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level.movingTiles) {
            this->CreateMovingTile(tile.position.x, tile.position.y,tile.size.x,tile.size.y, tile.connGroup,tile.velocity,tile.endpoint, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level->interactiveTiles) {
            this->CreateInteractive(tile.position.x, tile.position.y, tile.size.x, tile.size.y, tile.connGroup, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level->collectibles) {
            this->CreateCollectible(tile.position.x, tile.position.y,tile.size.x,tile.size.y, tile.connGroup, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level->damagingTiles) {
            this->CreateDamagingTile(tile.position.x, tile.position.y, tile.size.x, tile.size.y, tile.connGroup, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level->destroyTiles) {
            this->CreateDestroyTile(tile.position.x, tile.position.y, tile.size.x, tile.size.y, tile.connGroup, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level->checkpoints) {
            this->CreateCheckpoint(tile.position.x, tile.position.y, tile.size.x, tile.size.y, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level->levelExits) {
            this->CreateExit(tile.position.x, tile.position.y, tile.size.x, tile.size.y, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level.enemies) {
            this->CreateEnemy(tile.position.x, tile.position.y, tile.size.x, tile.size.y, this->level.textures2d.at(tile.texture));
        }
        this->level.finished = 0;
    }

    void World::ClearLevel() {
        this->level = Level();
    }

}