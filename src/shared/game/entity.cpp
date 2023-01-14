#include "entity.h"
#include "macros.h"
#include "config.h"
#include "input.h"
#include "serialization/bitstream.h"
#include "serialization/serialize.h"
#include "serialization/serialize_extra.h"
#include "protocol/protocol.h"

#include "raymath.h"
#include <algorithm>
#include <glm/glm.hpp>

#ifndef RESOURCE_PATH
/* VSCode doesn't notice RESOURCE_PATH defined from CMake and shows nonexistent errors as a result.
   This #define is here just to silence these errors.
*/
#define RESOURCE_PATH
#endif

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
            entity_textures[ENTITY_TYPE_TILE] = LoadTexture(RESOURCE_PATH "/tiles/tile.png");
            entity_textures[ENTITY_TYPE_INTERACTIVE] = LoadTexture(RESOURCE_PATH "/tiles/interactive.png");
            entity_textures[ENTITY_TYPE_MOVING_TILE] = LoadTexture(RESOURCE_PATH "/tiles/movingtile.png");
            entity_textures[ENTITY_TYPE_COLLECTIBLE] = LoadTexture(RESOURCE_PATH "/collectible.png");
            entity_textures[ENTITY_TYPE_DAMAGING_TILE] = LoadTexture(RESOURCE_PATH "/tiles/dmgtile.png");
            entity_textures[ENTITY_TYPE_DESTRUCTIBLE_TILE] = LoadTexture(RESOURCE_PATH "/tiles/destroytile.png");
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
        if (input->dash && dash_cooldown <= 0.0f && ability_reset && velocity.x != 0)
        {
            move_speed *= Const::PLAYER.DASH_SPEED;
            dash_cooldown = Const::PLAYER.DASH_COOLDOWN;
            ability_reset = false;
        }
        else if (move_speed > base_speed) {
            move_speed -= Const::PLAYER.DASH_BRAKESPEED;
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

    }
    void Entity::UpdateMovingTile(float dt) {
        Vector2 tmp_pos = Vector2Scale(velocity, dt);
        float dirx = 1;
        float diry = 1;
        move_direction.x = 0;
        move_direction.y = 0;
        if (active) {
            if (position.x != endpoints[target].x) {
                dirx = (position.x - endpoints[target].x) / abs(position.x - endpoints[target].x);
                move_direction.x = log10f(abs(position.x - endpoints[target].x) + 5) * dirx * -1.0f;
            }
            if (position.y != endpoints[target].y) {
                diry = (position.y - endpoints[target].y) / abs(position.y - endpoints[target].y);
                move_direction.y = log10f(abs(position.y - endpoints[target].y) + 5) * diry * -1.0f;
            }
            if (reachedEndpoint(endpoints[target], dt)) {
                target = !target;
                move_direction.x = move_direction.x * -1.0f;
                move_direction.y = move_direction.y * -1.0f;
            }
        }

        velocity = Vector2Scale(move_direction, move_speed);
        Vector2 delta_pos = Vector2Scale(velocity, dt);
        position = Vector2Add(position, delta_pos);

    }

    void Entity::Control(Input* input, float dt)
    {
        ASSERT(type == ENTITY_TYPE_PLAYER);
        move_direction.x = 0;
        setMoveSpeed(input);
        move_direction.x -= input->move[DIRECTION_LEFT];
        move_direction.x += input->move[DIRECTION_RIGHT];
        if (input->move[DIRECTION_UP] && on_ground)
            move_direction.y = (float)jump_height * -1.0f; 
        velocity.x = move_direction.x * move_speed;
        velocity.y = move_direction.y * Const::PLAYER.FALL_SPEED;
    }
    void Entity::MoveX(float dt) {
        ASSERT(type == ENTITY_TYPE_PLAYER || type == ENTITY_TYPE_ENEMY);
        position.x += velocity.x * dt;
    }
    void Entity::MoveY(float dt) {
        ASSERT(type == ENTITY_TYPE_PLAYER || type == ENTITY_TYPE_ENEMY);
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

    bool Entity::reachedEndpoint(Vector2 target,float dt) {

        if (endpoints[0].x == endpoints[1].x) {
            float distance1 = sqrtf(powf(abs(position.y - target.y), 2));
            if ((distance1 < 0.3)) {
                return 1;
            }
        }
        if (endpoints[0].y == endpoints[1].y) {
            float distance1 = sqrtf(powf(abs(position.x - target.x), 2));
            if ((distance1 < 0.3)) {
                return 1;
            }
        }
        else {
            float distance1 = sqrtf(powf(abs(position.y - target.y), 2) + powf(abs(position.x - target.x), 2));
            if ((distance1 < 0.3)) {
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

    void Entity::correctPositions(Entity ent,Vector2 velo, Vector2 ent_vel,float dt) {
        Vector2 ent_pos = {};
        Vector2 startPosition = position;
        ent_pos.x = ent.position.x + ent.velocity.x * dt;
        ent_pos.y = ent.position.y + ent.velocity.y * dt;
        if (ent_vel.y < 0) {
            if (velo.y > 0) {
                float offset = ent_pos.y - ent.position.y;
                position.y = ent.position.y - size.y + offset - 0.5f; //WORKS, BUT LIKE SHIT
                position.x += ent.velocity.x * dt;
                on_ground = true;
            }
            else if (velo.y < 0) {
                position.y = ent.position.y + ent.size.y;
                velocity.y = 0;
                move_direction.y = 0;
                collideTop = true;
            }
        }
        if (ent_vel.y > 0) {
            if (velo.y < 0) {
                float offset = ent_pos.y - ent.position.y;
                position.y = ent.position.y + size.y + offset;
                velocity.y = 0;
                move_direction.y = 0;
                collideTop = true;
            }
            else if (velo.y > 0) {
                float offset = ent_pos.y - ent.position.y;
                position.y = ent.position.y - size.y - offset;
                position.x += ent.velocity.x * dt;
                velocity.y = 0;
                on_ground = true;
            }
            else {
                collideTop = true;
            }
        }
        if (ent_vel.x > 0) {
            position.x = ent.position.x + ent.size.x;
            if (velo.x < 0) {
                velocity.x = 0;
                collideLeft = true;
            }
            else if (velo.x > 0) {
                velocity.x = 0;
                position.x = ent.position.x - size.x;
                collideRight = true;
            }
            else {
                collideLeft = true;
            }
        }
        if (ent_vel.x < 0) {
            position.x = ent.position.x - size.x;
            if (velo.x > 0) {
                velocity.x = 0;
                collideRight = true;
            }
            else if (velo.x < 0) {
                velocity.x = 0;
                position.x = ent.position.x + ent.size.x;
                collideLeft = true;
            }
            else {
                collideRight = true;
            }
        }
        if (ent_vel.y == 0 && ent_vel.x == 0) {
            if (velo.y < 0) {
                position.y = ent.position.y + ent.size.y;
                velocity.y = 0;
                move_direction.y = 0;
                collideTop = true;
            }
            else if (velo.y > 0) {
                position.y = ent.position.y - size.y;
                on_ground = true;
                position.x += ent.velocity.x * dt;
                ability_reset = true;
                velocity.y = 0;
            }
            if (velo.x > 0) {
                velocity.x = 0;
                position.x = ent.position.x - size.x;
                collideRight = true;
            }
            else if (velo.x < 0) {
                velocity.x = 0;
                position.x = ent.position.x + ent.size.x;
                collideLeft = true;
            }
            else if (velo.x == 0 && velo.y == 0) {
                collideTop = true;
            }
        }
        if (abs(startPosition.x - position.x) > (size.x) && dash_cooldown <= 0) {
            collideTop = true;
        }
    }

    void Entity::correctEnemyPositions(Entity ent, Vector2 velo, Vector2 ent_vel, float dt) {
        Vector2 ent_pos = {};
        Vector2 startPosition = position;
        ent_pos.x = ent.position.x + ent.velocity.x * dt;
        ent_pos.y = ent.position.y + ent.velocity.y * dt;
        if (ent_vel.y < 0) {
            if (velo.y > 0) {
                float offset = ent_pos.y - ent.position.y;
                position.y = ent.position.y - size.y + offset + ent.move_direction.y / 1.8f; //WORKS, BUT LIKE SHIT
                position.x += ent.velocity.x * dt;
                on_ground = true;
            }
            else if (velo.y < 0) {
                position.y = ent.position.y + ent.size.y;
                velocity.y = 0;
                move_direction.y = 0;
                collideTop = true;
            }
        }
        if (ent_vel.y > 0) {
            if (velo.y < 0) {
                float offset = ent_pos.y - ent.position.y;
                position.y = ent.position.y + size.y + offset;
                velocity.y = 0;
                move_direction.y = 0;
                collideTop = true;
            }
            else if (velo.y > 0) {
                float offset = ent_pos.y - ent.position.y;
                position.y = ent.position.y - size.y - offset;
                position.x += ent.velocity.x * dt;
                velocity.y = 0;
                on_ground = true;
            }
            else {
                collideTop = true;
            }
        }
        if (ent_vel.x > 0) {
            position.x = ent.position.x + ent.size.x;
            if (velo.x < 0) {
                velocity.x = velocity.x *-1;
                collideLeft = true;
            }
            else if (velo.x > 0) {
                velocity.x = velocity.x * -1;
                position.x = ent.position.x - size.x;
                collideRight = true;
            }
            else {
                collideLeft = true;
            }
        }
        if (ent_vel.x < 0) {
            position.x = ent.position.x - size.x;
            if (velo.x > 0) {
                velocity.x = velocity.x * -1;
                collideRight = true;
            }
            else if (velo.x < 0) {
                velocity.x = velocity.x * -1;
                position.x = ent.position.x + ent.size.x;
                collideLeft = true;
            }
            else {
                collideRight = true;
            }
        }
        if (ent_vel.y == 0 && ent_vel.x == 0) {
            if (velo.y < 0) {
                position.y = ent.position.y + ent.size.y;
                velocity.y = 0;
                move_direction.y = 0;
                collideTop = true;
            }
            else if (velo.y > 0) {
                position.y = ent.position.y - size.y;
                on_ground = true;
                position.x += ent.velocity.x * dt;
                ability_reset = true;
                velocity.y = 0;
            }
            if (velo.x > 0) {
                velocity.x = velocity.x * -1;
                position.x = ent.position.x - size.x;
                collideRight = true;
            }
            else if (velo.x < 0) {
                velocity.x = velocity.x* -1;
                position.x = ent.position.x + ent.size.x;
                collideLeft = true;
            }
            else if (velo.x == 0 && velo.y == 0) {
                collideTop = true;
            }
        }
        if (abs(startPosition.x - position.x) > (size.x) && dash_cooldown <= 0) {
            collideTop = true;
        }
    }
    void Entity::stalkPlayer(Entity ent, float dt) {
        float distance = sqrt(pow(position.x - ent.position.x, 2) + pow(position.y - ent.position.y, 2));
        if (distance < 70) {
            float direction = abs(position.x - ent.position.x)/(position.x-ent.position.x) * -1;
            velocity.x = 30.0f * direction;
        }
        else {
            velocity.x = abs(velocity.x) / velocity.x * Const::ENEMY.VELOCITY_X;
        }
    }

    float Entity::relativeHealth() const {
        if(base_health <= 0) return 0;
        return glm::clamp(health / static_cast<float>(base_health), 0.0f, 1.0f);
    }

    void Entity::drawHealthBar() const {

        constexpr float 
            barHeight = 3, 
            barGap = 3, 
            hueRed = 0, 
            hueGreen = 120;

        Color barColor = ColorFromHSV(glm::mix(hueRed, hueGreen, relativeHealth()), 1, 1);
        
        DrawRectangle(position.x, position.y + size.y + barGap, size.x, barHeight, BLACK);
        DrawRectangle(position.x, position.y + size.y + barGap, size.x * relativeHealth(), barHeight, barColor);
    }
}