#include "world.h"
#include "input.h"
#include <graphics/raylib_shaders.h>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include <string.h>
#include <macros.h>
#include <content/particles.h>
#include <math/geom.h>
#include <rng.h>


namespace Game {

    void World::ControlEnemy(Entity& enemy, float dt) {
        Vector2 ent_pos;
        float shot_cooldown = 0.0f;
        if (~enemy.variant&(ENEMY_MOVE)) {
            enemy.velocity.x = 0;
        }
        if (enemy.variant&(ENEMY_SHOOT_TOP)) {
            if (enemy.shot_cooldown <= 0) { //center out shot
                CreateBullet(enemy.position.x, enemy.position.y, 16, 16, 0, Const::ENTITY.BULLET_VELOCITY_Y,0);
                shot_cooldown = Const::ENEMY.SHOT_COOLDOWN;
                enemy.shot_cooldown = shot_cooldown;
            }
        }
        if (enemy.variant&(ENEMY_SHOOT_SIDE)) {
            if (enemy.shot_cooldown <= 0) { //center out shot
                CreateBullet(enemy.position.x, enemy.position.y, 16, 16, Const::ENTITY.BULLET_VELOCITY_X*enemy.facing, 0, 0);
                shot_cooldown = Const::ENEMY.SHOT_COOLDOWN;
                enemy.shot_cooldown = shot_cooldown;
            }
        }
        if (enemy.variant&(ENEMY_JUMP)) {
            if (enemy.time_until_state_change_allowed <= 0){
                enemy.move_direction.y = enemy.jump_height * -1.0f;
                enemy.time_until_state_change_allowed = Const::ENEMY.JUMP_COOLDOWN;
            }
        }
    }


    void World::CalculateCollisions(Entity& player, float dt, bool dim) {
        if (player.type == ENTITY_TYPE_PLAYER) {
            for (int collide_idx = 0; collide_idx < max_entity_count; collide_idx++)
            {
                Entity* collideEntity = entities + collide_idx;
                if (collideEntity->type != ENTITY_TYPE_NONE && collideEntity->collidable) {
                    Vector2 ent_vel = {};
                    Vector2 vel = {};
                    if (!dim) {
                        ent_vel.x = collideEntity->velocity.x;
                        ent_vel.y = 0;
                        vel.x = player.velocity.x;
                        vel.y = 0;
                    }
                    else {
                        ent_vel.x = 0;
                        ent_vel.y = collideEntity->velocity.y;
                        vel.x = 0;
                        vel.y = player.velocity.y;
                    }
                    if (player.collidesWith(*collideEntity)) {
                        player.correctPositions(*collideEntity, vel, ent_vel, dt);
                    }
                }
            }
        }
        else if (player.type == ENTITY_TYPE_ENEMY) {
            for (int collide_idx = 0; collide_idx < max_entity_count; collide_idx++)
            {
                Entity* collideEntity = entities + collide_idx;
                if (collideEntity->type != ENTITY_TYPE_NONE && collideEntity->collidable) {
                    Vector2 ent_vel = {};
                    Vector2 vel = {};
                    if (!dim) {
                        ent_vel.x = collideEntity->velocity.x;
                        ent_vel.y = 0;
                        vel.x = player.velocity.x;
                        vel.y = 0;
                    }
                    else {
                        ent_vel.x = 0;
                        ent_vel.y = collideEntity->velocity.y;
                        vel.x = 0;
                        vel.y = player.velocity.y;
                    }
                    if (player.collidesWith(*collideEntity)) {
                        player.correctEnemyPositions(*collideEntity, vel, ent_vel, dt);
                    }
                }
                if (player.variant & (ENEMY_FOLLOW) && collideEntity->type == ENTITY_TYPE_PLAYER) {
                    player.stalkPlayer(*collideEntity,dt);
                }
            }
        }
    }

    void World::CheckStates(Entity& player, Vector2 velocity, Input* input, float dt) {
        for (int collide_idx = 0; collide_idx < max_entity_count; collide_idx++)
        {
            Entity* collideEntity = entities + collide_idx; //TODO add reaction to squishing - dmg, teleport?
            if (collideEntity->type == ENTITY_TYPE_INTERACTIVE) {
                if (player.collidesWith(*collideEntity) && input->interact
                    && collideEntity->time_until_state_change_allowed <= 0.0f) {
                    collideEntity->active = !collideEntity->active;
                    UpdateActiveTiles(dt, collideEntity->entity_group);
                    collideEntity->time_until_state_change_allowed = Const::ENTITY.INTERACTIVE_STATE_COOLDOWN; // change if state is the same longer then 1/4 of sec
                }
            }
            else if (collideEntity->type == ENTITY_TYPE_COLLECTIBLE) {
                if (player.collidesWith(*collideEntity)) {
                    if (collideEntity->variant & (COLLECTIBLE_MONEY)) {
                        player.money_count++;
                        FreeEntity(collideEntity);
                    }
                    if (collideEntity->variant & (COLLECTIBLE_HEALTH)) {
                        if (player.health < player.base_health) {
                            player.health++;
                        }
                    }

                }
            }
            else if (collideEntity->type == ENTITY_TYPE_DAMAGING_TILE) {
                if (player.collidesWith(*collideEntity)
                    && player.time_until_state_change_allowed <= 0.0f) {
                    player.InflictDamage(*this, *collideEntity, collideEntity->damage);
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
                    player.InflictDamage(*this, *collideEntity, collideEntity->damage);
                }
            }
            else if (collideEntity->type == ENTITY_TYPE_BULLET && collideEntity->owner == 0) {
                if (player.collidesWith(*collideEntity)
                    && player.time_until_state_change_allowed <= 0.0f) {
                    player.InflictDamage(*this, *collideEntity, collideEntity->damage);
                    FreeEntity(collideEntity);
                }
            }
        }
    }

    void World::UpdateActiveTiles(float dt, int connGroup) {
        for (int entity_idx = 0; entity_idx < max_entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            if (entity->entity_group == connGroup && (entity->type != ENTITY_TYPE_INTERACTIVE && entity->type != ENTITY_TYPE_NONE)) {
                entity->active = !entity->active;
                if (entity->type == ENTITY_TYPE_TILE)
                    entity->collidable = !entity->collidable;
            }
        }
    }

    void World::SprayParticles(
        int count,
        const ParticleType *type,
        Vector2 position,
        Vector2 velocity, float velocitySpread,
        float minSize, float maxSize
    ) {
        if(type == nullptr) return;

        Particle particle;
        particle.type = type;
        particle.bounds = RectCentered(position, {});

        for(int i=0; i<count; ++i) {
            particle.velocity = Vector2Add(
                RandomVector2(0, velocitySpread),
                velocity
            );
            particle.bounds.width = particle.bounds.height = RandomFloat(minSize, maxSize);
            particle.angularVelocity = RandomFloat(-300, 300);
            queuedParticles.push_back(particle);
        }
    }

    void World::hitObstacles(Entity& bullet) {

        EntityType collidedEntityType = ENTITY_TYPE_NONE;

        for (int entity_idx = 0; entity_idx < max_entity_count; entity_idx++)
        {
            Entity* entity = entities + entity_idx;
            if ((entity->type == ENTITY_TYPE_DESTRUCTIBLE_TILE || entity->type == ENTITY_TYPE_ENEMY)
                && bullet.collidesWith(*entity)
                && bullet.owner != entity->owner) {

                entity->InflictDamage(*this, bullet, bullet.damage);

                collidedEntityType = entity->type;
                if (entity->health <= 0) {

                    FreeEntity(entity);
                }
                break;
            }
            if (entity->type != ENTITY_TYPE_NONE && entity->collidable && bullet.collidesWith(*entity)) {
                collidedEntityType = entity->type;
                break;
            }
        }

        if(collidedEntityType != ENTITY_TYPE_NONE) {

            CreateShockwave(
                bullet.GetCenter(),
                MAX(bullet.size.x, bullet.size.y)*6
            );

            if(collidedEntityType == ENTITY_TYPE_TILE) {
                SprayParticles(RandomInt(2,5), Particles::pebble, bullet.GetCenter(), {0,-40}, 40, 2, 4);
            }

            FreeEntity(&bullet);
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
            CreateBullet(player.position.x, player.position.y, 16, 16, x_vel, y_vel,player.owner);
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
        if (!entity.on_ground) {
            entity.move_direction.y += 0.75f;
        }
        if (entity.move_direction.x != 0.0f)
            entity.facing = (int)SIGNF(entity.move_direction.x);
        entity.Control(input, dt);
        CheckPlayerShot(entity, input, dt);

        entity.MoveX(dt);
        CalculateCollisions(entity, dt, 0);
        entity.on_ground = false;
        entity.MoveY(dt);
        CalculateCollisions(entity, dt, 1);
        CheckStates(entity, Vector2{ entity.velocity.x,entity.velocity.y }, input, dt);
        if ((entity.on_ground && entity.collideTop) || (entity.collideLeft && entity.collideRight)) {
            //entity.health = 0;
        }
        entity.collideLeft = false;
        entity.collideRight = false;
        entity.collideTop = false;
    }
    void World::MoveEnemy(Entity &entity,float dt) {
        entity.facing = (int)SIGNF(entity.velocity.x);

        if (!entity.on_ground) {
            entity.move_direction.y += 0.75f;
        }
        ControlEnemy(entity, dt);
        entity.MoveX(dt);
        CalculateCollisions(entity, dt, 0);
        entity.on_ground = 0;
        entity.velocity.y = entity.move_direction.y * Const::ENEMY.FALL_SPEED;
        entity.MoveY(dt);
        CalculateCollisions(entity, dt, 1);
    }

    void World::Update(Input* inputs, int num_inputs, float dt)
    {
        parallax_background.Update(dt);

        for (int i = 0; i < max_entity_count; i++)
        {
            Entity* entity = &entities[i];
            entity->Update(*this, dt);

            switch (entity->type)
            {
            case ENTITY_TYPE_PLAYER:
            {
                ASSERT(entity->owner >= 0);
                ASSERT(entity->owner < num_inputs);
                if (entity->owner >= 0 && entity->owner < num_inputs)
                {
                    MovePlayer(*entity, &inputs[entity->owner], dt);
                }
            } break;

            case ENTITY_TYPE_MOVING_TILE:
            {
                entity->UpdateMovingTile(dt);
            } break;

            case ENTITY_TYPE_ENEMY:
            {
                MoveEnemy(*entity, dt);
            } break;

            case ENTITY_TYPE_BULLET:
            {
                Vector2 delta_pos = Vector2Scale(entity->velocity, dt);
                entity->position = Vector2Add(entity->position, delta_pos);
                hitObstacles(*entity);
                if (entity->time_until_state_change_allowed <= 0)
                    FreeEntity(entity);
            } break;
            }
        }
    }

    void World::Draw(const Rectangle visibleArea, DrawQueue &dq) const
    {
        BeginShaderMode(Game::RaylibShaders::world);

        parallax_background.Draw(visibleArea);

        for (int entity_idx = 0; entity_idx < max_entity_count; entity_idx++)
        {
            const Entity* entity = &entities[entity_idx];
            if (entity->type != ENTITY_TYPE_NONE)
                entity->Draw(dq);
        }
        Game::RaylibShaders::worldSetDepth(1);

        rlDrawRenderBatchActive();
        EndShaderMode();
    }

    void World::DrawHealthBars() const {

        for (int entity_idx = 0; entity_idx < max_entity_count; entity_idx++)
        {
            const Entity* entity = &entities[entity_idx];
            if (entity->type == ENTITY_TYPE_NONE)
                continue;
            if (entity->base_health > 0)
                entity->drawHealthBar();
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
        if (entity && entity->type != ENTITY_TYPE_NONE)
            entity->Despawn();
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

    AddEntityResult World::CreateTile(float pos_x, float pos_y, float width, float height, int conGroup,bool enabled)
    {
        AddEntityResult result = AddEntity(ENTITY_TYPE_TILE, 0, pos_x, pos_y, width, height);
        if (result.entity)
        {
            Entity* entity = result.entity;
            entity->entity_group = conGroup;
            entity->collidable = enabled;
            entity->active = enabled;
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

    Entity* World::CreateMovingTile(float pos_x, float pos_y, float width, float height, int conGroup, Vector2 moveDirection, Vector2 endpoint,bool enabled)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_MOVING_TILE, 0, pos_x, pos_y, width, height).entity)
        {
            entity->move_direction.x = (pos_x - endpoint.x) * -1;
            entity->move_direction.y = (pos_y - endpoint.y) * -1;
            entity->move_speed = Const::ENTITY.MOVING_TILE_MOVE_SPEED;
            entity->entity_group = conGroup;
            entity->active = enabled;
            entity->endpoints[1] = Vector2{ pos_x, pos_y };
            entity->endpoints[0] = Vector2{ endpoint.x, endpoint.y };
            entity->collidable = 1;
        }
        return entity;
    }

    Entity* World::CreateCollectible(float pos_x, float pos_y, float width, float height, int conGroup,int32_t property)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_COLLECTIBLE, 0, pos_x, pos_y, width, height).entity)
        {
            entity->entity_group = conGroup;
            entity->active = true;
            entity->variant = property;
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
            entity->health = entity->base_health = Const::ENTITY.DESTROY_TILE_HEALTH;
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

    Entity* World::CreateEnemy(float pos_x, float pos_y, float width, float height,int32_t property,int health)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_ENEMY, 0, pos_x, pos_y, width, height).entity)
        {
            entity->velocity.x = Const::ENEMY.VELOCITY_X;
            entity->damage = Const::ENEMY.DAMAGE;
            entity->variant = property;
            entity->owner = 0;
            entity->move_speed = Const::ENEMY.MOVE_SPEED;
            entity->jump_height = Const::ENEMY.JUMP_HEIGHT;
            entity->health = entity->base_health = health;
        }
        return entity;
    }

    Entity* World::CreateBullet(float pos_x, float pos_y, float width, float height, float x_vel, float y_vel,int owner)
    {
        Entity* entity = nullptr;
        if (entity = AddEntity(ENTITY_TYPE_BULLET, 0, pos_x, pos_y, width, height).entity)
        {
            entity->velocity.x = x_vel;
            entity->velocity.y = y_vel;
            entity->damage = Const::ENTITY.BULLET_DAMAGE;
            entity->active = true;
            entity->time_until_state_change_allowed = Const::ENTITY.BULLET_LIFETIME;
            entity->owner = owner;
            entity->num_frames = 4;
            entity->max_frame_time = 0.1;
        }
        return entity;
    }

    Entity* World::CreateShockwave(Vector2 pos, float max_size)
    {
        Entity* entity = AddEntity(ENTITY_TYPE_SHOCKWAVE, 0, pos.x, pos.y, max_size, max_size).entity;
        if(entity) {
            entity->num_frames = 30;
            entity->current_frame = 6;
            entity->max_frame_time = 1/60.0;
        }
        return entity;
    }

}
