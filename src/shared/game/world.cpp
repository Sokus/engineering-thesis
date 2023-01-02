#include "world.h"
#include "raylib.h"
#include <string.h>

namespace Game {

    void World::CalculateCollisions(Entity& player, Input* input, float dt) {
        glm::vec2 starting_pos = player.position;
        for (int collide_idx = 0; collide_idx < entity_count; collide_idx++)
        {
            Entity* collideEntity = entities + collide_idx;
            if ((collideEntity->type == TILE || collideEntity->type == MOVING_TILE) && collideEntity->active) {
                glm::vec2 ent_pos = collideEntity->position + (collideEntity->velocity * dt);
                if (player.collidesWithY(*collideEntity, dt)) {
                    if (player.velocity.y < 0) {
                        player.position.y = ent_pos.y + collideEntity->height * collideEntity->scale;
                        player.velocity.y = 0;
                        player.move_direction.y = 0;
                    }
                    else if (player.velocity.y > 0 || collideEntity->velocity.y < 0) {
                        player.position.y = collideEntity->position.y - player.height * player.scale;
                        player.onGround = 1;
                        player.velocity.y = 0;
                    }
                }
                else if (player.collidesWithX(*collideEntity, dt)) {
                    if (player.velocity.x > 0) {
                        player.velocity.x = 0;
                        player.position.x = collideEntity->position.x - player.width * player.scale;
                    }
                    else if (player.velocity.x < 0) {
                        player.position.x = collideEntity->position.x + player.width * player.scale;
                        player.velocity.x = 0;
                    }
                    else {
                        if (collideEntity->velocity.x > 0)
                            player.position.x = ent_pos.x + player.width * player.scale;
                        else if (collideEntity->velocity.x < 0)
                            player.position.x = ent_pos.x - player.width * player.scale;
                    }
                }
            }
            else if (collideEntity->type == INTERACTIVE) {
                if (player.collidesWithY(*collideEntity, dt) && input->move[Input::Direction::INTERACT]
                    && collideEntity->stateChange > 15) { //change if state is the same longer then 1/4 of sec
                    collideEntity->active = abs(collideEntity->active - 1);
                    UpdateActiveTiles(dt, collideEntity->connectionGroup);
                    collideEntity->stateChange = 0;
                }
                collideEntity->stateChange++;
            }
            else if (collideEntity->type == COLLECTIBLE) {
                if (player.collidesWithY(*collideEntity, dt) && collideEntity->active) { 
                    collideEntity->active = 0;
                    player.moneyCount++;
                }
            }
            else if (collideEntity->type == DAMAGING_TILE) {
                if (player.collidesWithY(*collideEntity, dt)
                    && collideEntity->stateChange > 90) { //hurts every 1.5 of sec
                    player.health -= 20;
                    collideEntity->stateChange = 0;
                }
                collideEntity->stateChange++;
            }
        }
    }

    void World::UpdateActiveTiles(float dt, int connGroup) {
        for (int entity_idx = 0; entity_idx < entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            if (entity->type == TILE && entity->connectionGroup == connGroup) {
                entity->active = abs(entity->active - 1);
            }
        }
    }

    void World::Clear()
    {
        memset(entities, 0, sizeof(entities));
        level = {};
    }

    void World::Update(Input *input, float dt)
    {
        level.Update(dt);
        
        for (int entity_idx = 0; entity_idx < entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            entity->Update(dt);
            if (entity->type == PLAYER) {
                entity->Control(input,dt);
                entity->onGround = 0;
                CalculateCollisions(*entity, input, dt);
            }
        }

        //int noAliveBullets = 0;
        //for(int bulletIdx = 0; bulletIdx < bullets.size(); ++bulletIdx) 
        //{
        //    bullets[bulletIdx].Update(dt);
        //    if(bullets[bulletIdx].IsAlive()) 
        //    {
        //        bullets[noAliveBullets++] = bullets[bulletIdx];
        //    }
        //}
        //bullets.resize(noAliveBullets);
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

    Entity* World::CreatePlayer(float pos_x, float pos_y,Texture2D texture)
    {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::PLAYER;
            entity->position = glm::vec2(pos_x, pos_y);
            entity->move_speed = 100.0f;
            entity->jumpHeight = 10;
            entity->max_animation_frame_time = 0.25f;
            entity->scale = 4;
            entity->health = 200;
            entity->texture = texture;
        }
        return entity;
    }

    Entity* World::CreateTile(float pos_x, float pos_y,int conGroup, Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::TILE;
            entity->position = glm::vec2(pos_x, pos_y);
            entity->move_speed = 0.0f;
            entity->max_animation_frame_time = 0.25f;
            entity->scale = 4;
            entity->connectionGroup = conGroup;
            entity->texture = texture;
            if (entity->connectionGroup != 0) {
                entity->active = 0;
            }
            else {
                entity->active = 1;
            }
        }
        return entity;
    }
    Entity* World::CreateInteractive(float pos_x, float pos_y,int conGroup, Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::INTERACTIVE;
            entity->position = glm::vec2(pos_x, pos_y);
            entity->move_speed = 0.0f;
            entity->max_animation_frame_time = 1.0f;
            entity->scale = 4;
            entity->frameChange = 0;
            entity->connectionGroup = conGroup;
            entity->active = 0;
            entity->texture = texture;
        }
        return entity;
    }
    Entity* World::CreateMovingTile(float pos_x, float pos_y, int conGroup, glm::vec2 moveDirection, glm::vec2 border[2], Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::MOVING_TILE;
            entity->position = glm::vec2(pos_x, pos_y);
            entity->move_direction = moveDirection;
            entity->move_speed = 1.0f;
            entity->max_animation_frame_time = 1.0f;
            entity->scale = 4;
            entity->frameChange = 0;
            entity->connectionGroup = conGroup;
            entity->active = 1;
            entity->border[0] = border[0];
            entity->border[1] = border[1];
            entity->texture = texture;
        }
        return entity;
    }
    Entity* World::CreateCollectible(float pos_x, float pos_y, int conGroup, Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::COLLECTIBLE;
            entity->position = glm::vec2(pos_x, pos_y);
            entity->move_speed = 1.0f;
            entity->max_animation_frame_time = 1.0f;
            entity->scale = 4;
            entity->frameChange = 0;
            entity->connectionGroup = conGroup;
            entity->active = 1;
            entity->texture = texture;
        }
        return entity;
    }
    Entity* World::CreateDamagingTile(float pos_x, float pos_y, int conGroup, Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::DAMAGING_TILE;
            entity->position = glm::vec2(pos_x, pos_y);
            entity->move_speed = 0.0f;
            entity->max_animation_frame_time = 1.0f;
            entity->scale = 4;
            entity->frameChange = 0;
            entity->connectionGroup = conGroup;
            entity->active = 1;
            entity->texture = texture;
        }
        return entity;
    }
    Entity* World::CreateBullet(ReferenceFrame rframe,float dmg, Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::BULLET;
            entity->referenceFrame = rframe;
            entity->damage = dmg;
            entity->animationFrames = { 2,2 };
            entity->visibleSize = { 10,10 };
            entity->animationLength = 0.5;
            entity->gravity = -200;
            entity->maxLifetime = 1.5;
            entity->ln1MinusDragCoefficient = 0;
            entity->sizeKeyframes ={ 05,1.5,0.5f };
            entity->alphaKeyframes = { 1,1,0 };


            entity->move_speed = 0.0f;
            entity->max_animation_frame_time = 1.0f;
            entity->scale = 4;
            entity->frameChange = 0;
            entity->connectionGroup = 0;
            entity->active = 1;
            entity->texture = texture;
        }
        return entity;
    }


    void World::SetLevel(Level &level) {
        this->level = level;
        this->level.LoadTextures();
        for (auto& tile : level.tiles) {
            this->CreateTile(tile.position.x, tile.position.y, tile.connGroup, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level.movingTiles) {
            this->CreateMovingTile(tile.position.x, tile.position.y, tile.connGroup,tile.velocity,tile.border, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level.interactiveTiles) {
            this->CreateInteractive(tile.position.x, tile.position.y, tile.connGroup, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level.collectibles) {
            this->CreateCollectible(tile.position.x, tile.position.y, tile.connGroup, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level.damagingTiles) {
            this->CreateDamagingTile(tile.position.x, tile.position.y, tile.connGroup, this->level.textures2d.at(tile.texture));
        }
    }

    void World::ClearLevel() {
        this->level = Level();
    }

}