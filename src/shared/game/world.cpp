#include "world.h"
#include "input.h"

#include "raylib.h"
#include "raymath.h"

#include <string.h>
#include <macros.h>

namespace Game {

    void World::MoveEnemy(Entity& enemy, float dt) {
        Vector2 ent_pos;
        for (int collide_idx = 0; collide_idx < max_entity_count; collide_idx++)
        {
            Entity* collideEntity = entities + collide_idx;
            if (collideEntity->type != ENTITY_TYPE_NONE && collideEntity->collidable) {
                ent_pos = Vector2Add(collideEntity->position, (Vector2Scale(collideEntity->velocity, dt)));
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

    void World::CalculateCollisions(Entity& player, Vector2 velocity, Input* input, float dt, bool dim) {
        for (int collide_idx = 0; collide_idx < max_entity_count; collide_idx++)
        {
            Entity* collideEntity = entities + collide_idx; //TODO add reaction to squishing - dmg, teleport?
            if (collideEntity->type != ENTITY_TYPE_NONE && collideEntity->collidable) {
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
                    collideEntity->time_until_state_change_allowed = Const::ENTITY.INTERACTIVE_STATE_COOLDOWN; // change if state is the same longer then 1/4 of sec
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
                    && player.time_until_state_change_allowed <= 0.0f) {
                    player.health -= collideEntity->damage;
                    player.time_until_state_change_allowed = Const::PLAYER.DAMAGE_COOLDOWN; //hurts every 1.5 of sec
                }
            }
            else if (collideEntity->type == ENTITY_TYPE_CHECKPOINT) {
                if (player.collidesWith(*collideEntity) && !collideEntity->active) {
                    float pos_y = collideEntity->position.y + collideEntity->size.y - player.size.y;
                    collideEntity->active = true;
                    spawnpoint = Vector2{ collideEntity->position.x, pos_y };
                }
            }
            else if (collideEntity->type == ENTITY_TYPE_EXIT) {
                if (player.collidesWith(*collideEntity)) {
                    finished = true;
                }
            }
            else if (collideEntity->type == ENTITY_TYPE_ENEMY) {
                if (player.collidesWith(*collideEntity)
                    && player.time_until_state_change_allowed <= 0.0f) {
                    player.health -= collideEntity->damage;
                    player.time_until_state_change_allowed = Const::PLAYER.DAMAGE_COOLDOWN;
                }
            }
        }
        //printf("L:%d,R:%d,T:%d,D:%d\n", player.collideLeft, player.collideRight, player.collideTop, player.on_ground);
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
    void World::hitObstacles(Entity& bullet) {
        for (int entity_idx = 0; entity_idx < max_entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            if ((entity->type == ENTITY_TYPE_DESTRUCTIBLE_TILE || entity->type == ENTITY_TYPE_ENEMY) && bullet.collidesWith(*entity)) {
                entity->health -= bullet.damage;
                if (entity->health <= 0) {

                    FreeEntity(entity);
                }
                FreeEntity(&bullet);
            }
            if (entity->type != ENTITY_TYPE_NONE && entity->collidable && bullet.collidesWith(*entity)) {
                FreeEntity(&bullet);
            }
        }
    }

    void World::Clear()
    {
        memset(entities, 0, sizeof(max_entity_count * sizeof(Entity)));
        for (int entity_idx = 0; entity_idx < max_entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            entity->type = ENTITY_TYPE_NONE;
        }
        parallax_background.Clear();
    }
    void World::CheckPlayerShot(Entity& player, Input* input, float dt) { //Add to check which player shoot
        if (input->shoot && player.shot_cooldown <= 0.0f) {
            float x_vel = Const::ENTITY.BULLET_VELOCITY_X * player.facing;
            float y_vel = player.velocity.y * !player.on_ground * 0.5f;
            CreateBullet(player.position.x, player.position.y, 16, 16, x_vel, y_vel);
            player.shot_cooldown = Const::PLAYER.SHOT_COOLDOWN;
        }
    }

    void World::MovePlayer(Entity& entity, Input* input, float dt) {

        ASSERT(entity.type == ENTITY_TYPE_PLAYER);
        if (entity.health <= 0) {
            entity.position = spawnpoint; //Add some death animation
            entity.health = entity.base_health;
            return;
        }
        entity.Control(input, dt);
        CheckPlayerShot(entity, input, dt);

        entity.MoveX(dt);
        CalculateCollisions(entity, Vector2{ entity.velocity.x,0.0f }, input, dt, 0);
        entity.on_ground = 0;
        entity.MoveY(dt);
        CalculateCollisions(entity, Vector2{ 0.0f,entity.velocity.y }, input, dt, 1);
        if ((entity.on_ground && entity.collideTop) || (entity.collideLeft && entity.collideRight)) {
            entity.health = 0;
        }
        entity.collideLeft = false;
        entity.collideRight = false;
        entity.collideTop = false;
    }

    void World::Update(Input* inputs, int num_inputs, float dt)
    {
        parallax_background.Update(dt);

        for (int i = 0; i < max_entity_count; i++)
        {
            Entity* entity = &entities[i];
            entity->Update(dt);

            switch (entity->type)
            {
            case ENTITY_TYPE_PLAYER:
            {
                ASSERT(entity->owner >= 0);
                ASSERT(entity->owner < num_inputs);
                if (entity->owner >= 0 && entity->owner < num_inputs)
                    MovePlayer(*entity, &inputs[entity->owner], dt);
            } break;

            case ENTITY_TYPE_ENEMY:
                MoveEnemy(*entity, dt);
                break;

            case ENTITY_TYPE_BULLET:
            {
                hitObstacles(*entity);
                if (entity->time_until_state_change_allowed <= 0)
                    FreeEntity(entity);
            } break;
            }
        }
    }

    void World::Draw()
    {
        // TODO(sokus): Fix this
        parallax_background.Draw({ 0, 0 });

        for (int entity_idx = 0; entity_idx < max_entity_count; entity_idx++)
        {
            Entity* entity = &entities[entity_idx];
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

    EntityReference World::GetOwnedEntityReference(EntityType type, int owner, int start_index)
    {
        ASSERT(start_index >= 0);

        EntityReference reference = {};

        for (int i = MAX(1, start_index); i < max_entity_count; i++)
        {
            Entity* entity = &entities[i];
            if (entity->owner != owner)
                continue;

            if (type == ENTITY_TYPE_NONE || type == entity->type)
            {
                reference.index = i;
                reference.revision = entity->revision;
                break;
            }
        }

        return reference;
    }

    bool World::EntityReferenceIsValid(EntityReference reference)
    {
        bool result = false;
        if (reference.index > 0 && reference.index < max_entity_count)
            result = entities[reference.index].revision == reference.revision;
        return result;
    }

    Entity* World::GetEntityByReference(EntityReference reference)
    {
        Entity* entity = nullptr;
        if (reference.index > 0 && reference.index < max_entity_count)
        {
            if (entities[reference.index].revision == reference.revision)
                entity = &entities[reference.index];
        }
        return entity;
    }

    AddEntityResult World::AddEntity(EntityType type, int owner, float x, float y, float w, float h)
    {
        AddEntityResult result = {};
        EntityReference reference = GetFreeEntityReference();
        Entity* entity = GetEntityByReference(reference);

        if (entity)
        {
            memset(entity, 0, sizeof(Entity));
            entity->type = type;
            entity->owner = owner;
            entity->position = Vector2{ x, y };
            entity->size.x = w;
            entity->size.y = h;

            result.reference = reference;
            result.entity = entity;
        }

        return result;
    }

    void World::FreeEntity(Entity* entity)
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

    void World::FreeOwnedEntities(int owner)
    {
        ASSERT(owner >= 0);
        for (int i = 0; i < Game::max_entity_count; i++)
        {
            if (entities[i].type != ENTITY_TYPE_NONE && entities[i].owner == owner)
                FreeEntity(&entities[i]);
        }
    }

    AddEntityResult World::CreatePlayer(int owner, float pos_x, float pos_y, Game::PlayerType playertype)
    {
        AddEntityResult result = AddEntity(ENTITY_TYPE_PLAYER, owner, pos_x, pos_y, 16, 24);
        if (result.entity)
        {
            Entity* entity = result.entity;
            entity->num_frames = 4;
            entity->max_frame_time = Const::ENTITY.MAX_FRAME_TIME;
            entity->collidable = 0;
            switch (playertype)
            {
            case PLAYER_TYPE_ROUGE:
                entity->base_health = Const::PLAYER.ROUGE.BASE_HEALTH;
                entity->base_speed = Const::PLAYER.ROUGE.BASE_SPEED;
                entity->jump_height = Const::PLAYER.ROUGE.JUMP_HEIGHT;
                break;
            case PLAYER_TYPE_SNIPER:
                entity->base_health = Const::PLAYER.SNIPER.BASE_HEALTH;
                entity->base_speed = Const::PLAYER.SNIPER.BASE_SPEED;
                entity->jump_height = Const::PLAYER.SNIPER.JUMP_HEIGHT;
                break;
            case PLAYER_TYPE_HEALER:
                entity->base_health = Const::PLAYER.HEALER.BASE_HEALTH;
                entity->base_speed = Const::PLAYER.HEALER.BASE_SPEED;
                entity->jump_height = Const::PLAYER.HEALER.JUMP_HEIGHT;
                break;
            case PLAYER_TYPE_WARRIOR:
                entity->base_health = Const::PLAYER.HEALER.BASE_HEALTH;
                entity->base_speed = Const::PLAYER.HEALER.BASE_SPEED;
                entity->jump_height = Const::PLAYER.WARRIOR.JUMP_HEIGHT;
                break;
            default: break;
            }
            entity->move_speed = entity->base_speed;
            entity->health = entity->base_health;
        }
        return result;
    }

    AddEntityResult World::CreateTile(float pos_x, float pos_y, float width, float height, int conGroup)
    {
        AddEntityResult result = AddEntity(ENTITY_TYPE_TILE, 0, pos_x, pos_y, width, height);
        if (result.entity)
        {
            Entity* entity = result.entity;
            entity->entity_group = conGroup;
            entity->collidable = true;
            entity->active = true;
        }
        return result;
    }

    Entity* World::CreateInteractive(float pos_x, float pos_y, float width, float height, int conGroup)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_INTERACTIVE, 0, pos_x, pos_y, width, height).entity)
        {
            entity->entity_group = conGroup;
        }
        return entity;
    }

    Entity* World::CreateMovingTile(float pos_x, float pos_y, float width, float height, int conGroup, Vector2 moveDirection, Vector2 endpoint)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_MOVING_TILE, 0, pos_x, pos_y, width, height).entity)
        {
            entity->move_direction.x = (pos_x - endpoint.x) * -1;
            entity->move_direction.y = (pos_y - endpoint.y) * -1;
            entity->move_speed = Const::ENTITY.MOVING_TILE_MOVE_SPEED;
            entity->entity_group = conGroup;
            entity->active = true;
            entity->endpoints[1] = Vector2{ pos_x, pos_y };
            entity->endpoints[0] = Vector2{ endpoint.x, endpoint.y };
            entity->collidable = true;
        }
        return entity;
    }

    Entity* World::CreateCollectible(float pos_x, float pos_y, float width, float height, int conGroup)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_COLLECTIBLE, 0, pos_x, pos_y, width, height).entity)
        {
            entity->entity_group = conGroup;
            entity->active = true;
        }
        return entity;
    }

    Entity* World::CreateDamagingTile(float pos_x, float pos_y, float width, float height, int conGroup)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_DAMAGING_TILE, 0, pos_x, pos_y, width, height).entity)
        {
            entity->entity_group = conGroup;
            entity->active = true;
            entity->damage = Const::ENTITY.DAMAGING_TILE_DAMAGE;
        }
        return entity;
    }

    Entity* World::CreateDestroyTile(float pos_x, float pos_y, float width, float height, int conGroup)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_DESTRUCTIBLE_TILE, 0, pos_x, pos_y, width, height).entity)
        {
            entity->entity_group = conGroup;
            entity->active = true;
            entity->health = Const::ENTITY.DESTROY_TILE_HEALTH;
            entity->collidable = 1;
        }
        return entity;
    }

    Entity* World::CreateCheckpoint(float pos_x, float pos_y, float width, float height)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_CHECKPOINT, 0, pos_x, pos_y, width, height).entity)
        {

        }
        return entity;
    }

    Entity* World::CreateExit(float pos_x, float pos_y, float width, float height)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_EXIT, 0, pos_x, pos_y, width, height).entity)
        {

        }
        return entity;
    }

    Entity* World::CreateEnemy(float pos_x, float pos_y, float width, float height)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_ENEMY, 0, pos_x, pos_y, width, height).entity)
        {
            entity->velocity.x = Const::ENEMY.VELOCITY_X;
            entity->damage = Const::ENEMY.DAMAGE;
        }
        return entity;
    }

    Entity* World::CreateBullet(float pos_x, float pos_y, float width, float height, float x_vel, float y_vel)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_BULLET, 0, pos_x, pos_y, width, height).entity)
        {
            entity->velocity.x = x_vel;
            entity->velocity.y = y_vel;
            entity->damage = Const::ENTITY.BULLET_DAMAGE;
            entity->active = true;
            entity->time_until_state_change_allowed = Const::ENTITY.BULLET_LIFETIME;
        }
        return entity;
    }

}
