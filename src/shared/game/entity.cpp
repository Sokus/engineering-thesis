#include "entity.h"
#include "macros.h"
#include "config.h"
#include "input.h"
#include "serialization/bitstream.h"
#include "serialization/serialize.h"
#include "serialization/serialize_extra.h"
#include "protocol/protocol.h"

#include "raylib.h"
#include "raymath.h"

namespace Game {

    Texture entity_textures[ENTITY_TYPE_COUNT];
    bool entity_textures_loaded = false;

    void LoadEntityTextures()
    {
        ASSERT(!entity_textures_loaded);
        if (!entity_textures_loaded)
        {
            // TODO: stub texture
            // entity_textures[ENTITY_TYPE_NONE] = LoadTexture(RESOURCE_PATH "");
            entity_textures[ENTITY_TYPE_PLAYER] = LoadTexture(RESOURCE_PATH "/character.png");
            entity_textures[ENTITY_TYPE_TILE] = LoadTexture(RESOURCE_PATH "/tile.png");
            entity_textures[ENTITY_TYPE_INTERACTIVE] = LoadTexture(RESOURCE_PATH "/interactive.png");
            entity_textures[ENTITY_TYPE_MOVING_TILE] = LoadTexture(RESOURCE_PATH "/movingtile.png");
            entity_textures[ENTITY_TYPE_COLLECTIBLE] = LoadTexture(RESOURCE_PATH "/collectible.png");
            entity_textures[ENTITY_TYPE_DAMAGING_TILE] = LoadTexture(RESOURCE_PATH "/dmgtile.png");
            entity_textures[ENTITY_TYPE_DESTRUCTIBLE_TILE] = LoadTexture(RESOURCE_PATH "/destroytile.png");
            entity_textures[ENTITY_TYPE_CHECKPOINT] = LoadTexture(RESOURCE_PATH "/checkpoint.png");
            entity_textures[ENTITY_TYPE_ENEMY] = LoadTexture(RESOURCE_PATH "/enemy.png");
            entity_textures[ENTITY_TYPE_BULLET] = LoadTexture(RESOURCE_PATH "/ballLightning.2x2.png");
            entity_textures[ENTITY_TYPE_EXIT] = LoadTexture(RESOURCE_PATH "/exit.png");
            entity_textures_loaded = true;
        }
    }

    Texture GetEntityTexture(EntityType type)
    {
        ASSERT(entity_textures_loaded);
        ASSERT(type >= 0);
        ASSERT(type < ENTITY_TYPE_COUNT);
        if (type >= 0 && type < ENTITY_TYPE_COUNT)
        {
            return entity_textures[type];
        }
        return entity_textures[ENTITY_TYPE_NONE];
    }

    void Entity::setMoveSpeed(Input* input) {
        if (input->dash && dash_cooldown <= 0.0f && ability_reset)
        {
            move_speed *= 10.0f;
            dash_cooldown = 1.5f;
            ability_reset = false;
        }
        else if (move_speed > base_speed) {
            move_speed -= 100.0f;
        }
        else {
            move_speed = base_speed;
        }
    }

	void Entity::Update(float dt)
    {
        if (num_frames)
        {
            frame_time += dt;

            if (frame_time > max_frame_time)
            {
                current_frame = (current_frame + 1) % num_frames;
                frame_time -= max_frame_time;
            }
        }

        if (time_until_state_change_allowed > 0.0f)
        {
            time_until_state_change_allowed -= dt;
            if (time_until_state_change_allowed < 0.0f)
                time_until_state_change_allowed = 0.0f;
        }
        if (shot_cooldown > 0.0f)
        {
            shot_cooldown -= dt;
            if (shot_cooldown < 0.0f)
                shot_cooldown = 0.0f;
        }

        if (dash_cooldown > 0.0f)
        {
            dash_cooldown -= dt;
            if (dash_cooldown < 0.0f)
                dash_cooldown = 0.0f;
        }

        if (type == ENTITY_TYPE_MOVING_TILE) {
            Vector2 tmp_pos = Vector2Scale(velocity, dt);
            float dirx = 1;
            float diry = 1;
            move_direction.x = 0;
            move_direction.y = 0;
            if (position.x != endpoints[target].x) {
                dirx = (position.x - endpoints[target].x) / abs(position.x - endpoints[target].x);
                move_direction.x = log10f(abs(position.x - endpoints[target].x)+5) * dirx * -1.0f;
            }
            if (position.y != endpoints[target].y) {
                diry = (position.y - endpoints[target].y) / abs(position.y - endpoints[target].y);
                move_direction.y = log10f(abs(position.y - endpoints[target].y)+5) * diry * -1.0f;
            }
            if (reachedEndpoint(endpoints[target], dt)) {
                target = !target;
                move_direction.x = move_direction.x * -1.0f;
                move_direction.y = move_direction.y * -1.0f;
            }

            velocity = Vector2Scale(move_direction, move_speed);
            Vector2 delta_pos = Vector2Scale(velocity, dt);
            position = Vector2Add(position, delta_pos);
        }
        if (type == ENTITY_TYPE_ENEMY) {
            Vector2 delta_pos = Vector2Scale(velocity, dt);
            position = Vector2Add( position,delta_pos);
        }
        if (type == ENTITY_TYPE_BULLET) {
            Vector2 delta_pos = Vector2Scale(velocity, dt);
            position = Vector2Add(position, delta_pos);
        }
    }

    void Entity::Control(Input* input, float dt)
    {
        ASSERT(type == ENTITY_TYPE_PLAYER);
        move_direction.x = 0;
        move_direction.x -= input->move[DIRECTION_LEFT] * 2.0f;
        move_direction.x += input->move[DIRECTION_RIGHT] * 2.0f;
        setMoveSpeed(input);
        if (move_direction.x != 0.0f)
            facing = (int)move_direction.x;
        if (input->move[DIRECTION_UP] && on_ground)
            move_direction.y = (float)jump_height * -1.0f;
        if (!on_ground) {
            move_direction.y += 0.75f;
        }
        velocity.x = move_direction.x * move_speed;
        velocity.y = move_direction.y * 30.0f;
    }
    void Entity::MoveX(float dt) {
        ASSERT(type == ENTITY_TYPE_PLAYER);
        position.x += velocity.x * dt;
    }
    void Entity::MoveY(float dt) {
        ASSERT(type == ENTITY_TYPE_PLAYER);
        position.y += velocity.y * dt;
    }

    void Entity::Draw()
    {
        Rectangle source = {};
        source.width = facing >= 0 ? size.x : -size.x;
        source.height = size.y;

        if (num_frames)
        {
            source.x = (float)(current_frame * size.x);
        }

        switch (type)
        {
            case ENTITY_TYPE_PLAYER:
            {
                source.y += (ABSF(velocity.x) <= 0.001f ? 0.0f : size.y);
            } break;

            case ENTITY_TYPE_INTERACTIVE:
            {
                if (active)
                    source.x = size.x;
            } break;

            case ENTITY_TYPE_TILE:
            {
                if (!active)
                    return;
            } break;

            case ENTITY_TYPE_NONE:
            {
                return;
            } break;

            default: break;
        }

        Rectangle destination = {};
        destination.x = position.x;
        destination.y = position.y;
        destination.width = size.x;
        destination.height = size.y;

        Texture texture = GetEntityTexture(type);
        DrawTexturePro(texture, source, destination, Vector2{}, 0.0f, WHITE);
    }

    bool Entity::collidesWith(Entity ent) {
        if (position.x + size.x > ent.position.x && position.x < ent.position.x + ent.size.x &&
            position.y + size.y > ent.position.y && position.y < ent.position.y + ent.size.y) {
            return 1;
        }
        return 0;
    }
    void Entity::calculateCollisionSide(Entity ent) {
        if ((position.x+size.x/2) - (ent.position.x+ent.size.x/2) > 0) {
            collideLeft = true;
        }
        if ((position.x + size.x / 2) - (ent.position.x + ent.size.x / 2) < 0) {
            collideRight = true;
        }
        if ((position.y + size.y / 2) - (ent.position.y + ent.size.y / 2) > 0) {
            collideTop = true;
        }
    }

    bool Entity::reachedEndpoint(Vector2 target,float dt) {

        if (endpoints[0].x == endpoints[1].x) {
            float distance1 = sqrtf(powf(abs(position.y - target.y), 2));
            if ((distance1 < 0.1)) {
                return 1;
            }
        }
        if (endpoints[0].y == endpoints[1].y) {
            float distance1 = sqrtf(powf(abs(position.x - target.x), 2));
            if ((distance1 < 0.05)) {
                return 1;
            }
        }
        else {
            float distance1 = sqrtf(powf(abs(position.y - target.y), 2) + powf(abs(position.x - target.x), 2));
            if ((distance1 < 0.1)) {
                return 1;
            }
        }
        return 0;
    }

    bool Entity::Serialize(BitStream *stream)
    {
        SERIALIZE_ENUM(stream, type, EntityType, ENTITY_TYPE_COUNT - 1);
        SERIALIZE_BITS(stream, revision, 16);
        if (type != ENTITY_TYPE_NONE)
        {
            SERIALIZE_INT(stream, owner, 0, MAX_CLIENTS - 1);
            SERIALIZE_INT(stream, current_frame, 0, 7);
            SERIALIZE_VECTOR2(stream, size);
            SERIALIZE_VECTOR2(stream, position);
        }
        return true;
    }
}