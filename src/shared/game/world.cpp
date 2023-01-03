#include "world.h"
#include "raylib.h"
#include <string.h>

namespace Game {
    Level ActualLevel;
    PlayerType ActualPlayer;

    void World::CalculateCollisions(Entity& player,glm::vec2 velocity, Input* input, float dt,bool dim) {
        glm::vec2 ent_pos;
        glm::vec2 ent_vel;
        for (int collide_idx = 0; collide_idx < entity_count; collide_idx++)
        {
            Entity* collideEntity = entities + collide_idx; //TODO add reaction to squishing - dmg, teleport?
            if (collideEntity->type!= NONE && collideEntity->collidable) {
                ent_pos = collideEntity->rF.position + (collideEntity->rF.velocity * dt);
                if (dim == 0) {
                    ent_vel = glm::vec2(collideEntity->rF.velocity.x, 0.0f);
                }
                else {
                    ent_vel = glm::vec2(0.0f,collideEntity->rF.velocity.y);
                }
                if (player.collidesWith(*collideEntity)) {
                    if (velocity.x > 0) {
                        player.rF.velocity.x = 0;
                        player.rF.position.x = ent_pos.x - player.width * player.scale;
                    }
                    if (velocity.x < 0) {
                        player.rF.velocity.x = 0;
                        player.rF.position.x = ent_pos.x + collideEntity->width * collideEntity->scale;
                    }
                    if (velocity.y < 0) {
                        player.rF.position.y = ent_pos.y + collideEntity->height * collideEntity->scale;
                        player.rF.velocity.y = 0;
                    }
                    if (velocity.y > 0) {
                        player.rF.position.y = ent_pos.y - player.height * player.scale;
                        player.playerData.onGround = 1;
                        player.playerData.ability_reset = 1;
                        player.rF.velocity.y = 0;
                        player.move_direction.y = 0.5;
                    }
                    if (velocity.y < 0 && ent_vel.y != 0) { 
                        player.rF.position.y = ent_pos.y + collideEntity->height * collideEntity->scale;
                        player.rF.velocity.y = 0;
                        player.move_direction.y = 1;
                    }
                    if (ent_vel.x > 0)
                        player.rF.position.x = ent_pos.x + player.width * player.scale;
                    if (ent_vel.x < 0)
                        player.rF.position.x = ent_pos.x - player.width * player.scale;
                   
                }
            }
            else if (collideEntity->type == INTERACTIVE) {
                if (player.collidesWith(*collideEntity) && input->interact
                    && collideEntity->stateChange > 15) { //change if state is the same longer then 1/4 of sec
                    collideEntity->enabled = abs(collideEntity->enabled - 1);
                    UpdateActiveTiles(dt, collideEntity->connectionGroup);
                    collideEntity->stateChange = 0;
                }
                collideEntity->stateChange++;
            }
            else if (collideEntity->type == COLLECTIBLE) {
                if (player.collidesWith(*collideEntity) && collideEntity->visible) {
                    collideEntity->visible = 0;
                    player.playerData.moneyCount++;
                }
            }
            else if (collideEntity->type == DAMAGING_TILE) {
                if (player.collidesWith(*collideEntity)
                    && collideEntity->stateChange > 90) { //hurts every 1.5 of sec
                    player.health -= collideEntity->damage;
                    collideEntity->stateChange = 0;
                }
                collideEntity->stateChange++;
            }
            else if (collideEntity->type == CHECKPOINT) {
                if (player.collidesWith(*collideEntity) && !collideEntity->enabled) {
                    float pos_y = collideEntity->rF.position.y + collideEntity->height - player.height;
                    collideEntity->enabled = 1;
                    level.spawnpoint = glm::vec2(collideEntity->rF.position.x, pos_y);
                }
            }
            else if (collideEntity->type == EXIT) {
                if (player.collidesWith(*collideEntity)) {
                    level.finished = 1;
                }
            }
        }
    }

    void World::UpdateActiveTiles(float dt, int connGroup) {
        for (int entity_idx = 0; entity_idx < entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            if (entity->type == TILE && entity->connectionGroup == connGroup) {
                entity->enabled = abs(entity->enabled - 1);
                entity->visible = abs(entity->visible - 1);
                entity->collidable = abs(entity->collidable - 1);
            }
        }
    }
    void World::hitObstacles(Entity &bullet) {
        for (int entity_idx = 0; entity_idx < entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            if (entity->type == DESTROY_TILE && bullet.collidesWith(*entity) ) {
                entity->health -= bullet.damage;
                if (entity->health <= 0) {
                    entity->type = NONE;
                }
                bullet.type = NONE;
            }
        }
    }

    void World::Clear()
    {
        for (int entity_idx = 0; entity_idx < entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            entity->type = NONE;
        }
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
                if (entity->health <= 0) {
                    entity->rF.position = level.spawnpoint; //Add some death animation
                    entity->health = entity->base_health;
                    break;
                }
                entity->Control(input,dt);
                entity->MoveX(dt);
                CalculateCollisions(*entity,glm::vec2(entity->rF.velocity.x,0.0f), input, dt,0);
                entity->playerData.onGround = 0;
                entity->MoveY(dt);
                CalculateCollisions(*entity, glm::vec2(0.0f,entity->rF.velocity.y), input, dt,1);
            }
            if (entity->type == BULLET) {
                entity->rF.position = entity->rF.position;
                hitObstacles(*entity);
            }
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

    Entity* World::CreatePlayer(float pos_x, float pos_y,Texture2D texture,Game::PlayerType playertype)
    {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::PLAYER;
            entity->rF.position = glm::vec2(pos_x, pos_y);
            entity->max_animation_frame_time = 0.25f;
            entity->scale = 4;
            entity->texture = texture;
            entity->collidable = 0;
            switch (playertype)
            {
            case ROUGE: 
                entity->base_health = 80;
                entity->base_speed = 220.0f;
                entity->playerData.jumpHeight = 12;
                break;
            case SNIPER:
                entity->base_health = 70;
                entity->base_speed = 200.0f;
                entity->playerData.jumpHeight = 12;
                break;
            case HEALER:
                entity->base_health = 100;
                entity->base_speed = 180.0f;
                entity->playerData.jumpHeight = 10;
                break;
            case WARRIOR:
                entity->base_health = 120;
                entity->base_speed = 150.0f;
                entity->playerData.jumpHeight = 9;
                break;
            default: break;
            }
            entity->move_speed = entity->base_speed;
            entity->health = entity->base_health;
        }
        return entity;
    }

    Entity* World::CreateTile(float pos_x, float pos_y,int conGroup, Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::TILE;
            entity->rF.position = glm::vec2(pos_x, pos_y);
            entity->max_animation_frame_time = 0.25f;
            entity->scale = 4;
            entity->connectionGroup = conGroup;
            entity->texture = texture;
            entity->visible = 1;
            entity->collidable = 1;
            entity->enabled = entity->visible;
        }
        return entity;
    }
    Entity* World::CreateInteractive(float pos_x, float pos_y,int conGroup, Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::INTERACTIVE;
            entity->rF.position = glm::vec2(pos_x, pos_y);
            entity->max_animation_frame_time = 1.0f;
            entity->scale = 4;
            entity->frameChange = 0;
            entity->connectionGroup = conGroup;
            entity->enabled = 0;
            entity->texture = texture;
            entity->visible = 1;
            entity->collidable = 0;
        }
        return entity;
    }
    Entity* World::CreateMovingTile(float pos_x, float pos_y, int conGroup, glm::vec2 moveDirection, glm::vec2 border[2], Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::MOVING_TILE;
            entity->rF.position = glm::vec2(pos_x, pos_y);
            entity->move_direction = moveDirection;
            entity->max_animation_frame_time = 1.0f;
            entity->scale = 4;
            entity->move_speed = 1.0f;
            entity->frameChange = 0;
            entity->connectionGroup = conGroup;
            entity->enabled = 1;
            entity->visible = 1;
            entity->border[0] = border[0];
            entity->border[1] = border[1];
            entity->texture = texture;
            entity->collidable = 1;
        }
        return entity;
    }
    Entity* World::CreateCollectible(float pos_x, float pos_y, int conGroup, Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::COLLECTIBLE;
            entity->rF.position = glm::vec2(pos_x, pos_y);
            entity->max_animation_frame_time = 1.0f;
            entity->scale = 4;
            entity->frameChange = 0;
            entity->connectionGroup = conGroup;
            entity->enabled = 1;
            entity->visible = 1;
            entity->texture = texture;
            entity->collidable = 0;
        }
        return entity;
    }
    Entity* World::CreateDamagingTile(float pos_x, float pos_y, int conGroup, Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::DAMAGING_TILE;
            entity->rF.position = glm::vec2(pos_x, pos_y);
            entity->max_animation_frame_time = 1.0f;
            entity->scale = 4;
            entity->frameChange = 0;
            entity->connectionGroup = conGroup;
            entity->enabled = 1;
            entity->visible = 1;
            entity->texture = texture;
            entity->collidable = 0;
            entity->damage = 10;
        }
        return entity;
    }
    Entity* World::CreateBullet(ReferenceFrame rframe,float dmg, Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::BULLET;
            entity->rF = rframe;
            entity->damage = dmg;
            entity->bulletData.animationFrames = { 2,2 };
            entity->bulletData.visibleSize = { 10,10 };
            entity->bulletData.animationLength = 0.5;
            entity->bulletData.gravity = -200;
            entity->bulletData.maxLifetime = 1.5;
            entity->bulletData.ln1MinusDragCoefficient = 0;
            entity->bulletData.sizeKeyframes ={ 05,1.5,0.5f };
            entity->bulletData.alphaKeyframes = { 1,1,0 };


            entity->max_animation_frame_time = 1.0f;
            entity->scale = 4;
            entity->frameChange = 0;
            entity->connectionGroup = 0;
            entity->enabled = 1;
            entity->visible = 1;
            entity->texture = texture;
            entity->collidable = 0;
        }
        return entity;
    }
    Entity* World::CreateDestroyTile(float pos_x, float pos_y, int conGroup, Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::DESTROY_TILE;
            entity->rF.position = glm::vec2(pos_x, pos_y);
            entity->max_animation_frame_time = 1.0f;
            entity->scale = 4;
            entity->frameChange = 0;
            entity->connectionGroup = conGroup;
            entity->enabled = 1;
            entity->visible = 1;
            entity->texture = texture;
            entity->health = 15;
            entity->collidable = 1;
        }
        return entity;
    }
    Entity* World::CreateCheckpoint(float pos_x, float pos_y, Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::CHECKPOINT;
            entity->rF.position = glm::vec2(pos_x, pos_y);
            entity->max_animation_frame_time = 1.0f;
            entity->scale = 4;
            entity->frameChange = 0;
            entity->connectionGroup = 0;
            entity->enabled = 0;
            entity->texture = texture;
            entity->visible = 1;
            entity->collidable = 0;
        }
        return entity;
    }
    Entity* World::CreateExit(float pos_x, float pos_y, Texture2D texture) {
        Entity* entity = nullptr;
        if (entity = GetNewEntity())
        {
            entity->type = EntityType::EXIT;
            entity->rF.position = glm::vec2(pos_x, pos_y);
            entity->max_animation_frame_time = 1.0f;
            entity->scale = 4;
            entity->frameChange = 0;
            entity->enabled = 0;
            entity->texture = texture;
            entity->visible = 1;
            entity->collidable = 0;
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
        for (auto& tile : level.destroyTiles) {
            this->CreateDestroyTile(tile.position.x, tile.position.y, tile.connGroup, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level.checkpoints) {
            this->CreateCheckpoint(tile.position.x, tile.position.y, this->level.textures2d.at(tile.texture));
        }
        for (auto& tile : level.levelExits) {
            this->CreateExit(tile.position.x, tile.position.y, this->level.textures2d.at(tile.texture));
        }
        this->level.finished = 0;
    }

    void World::ClearLevel() {
        this->level = Level();
    }

}