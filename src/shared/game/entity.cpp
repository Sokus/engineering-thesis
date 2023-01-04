#include "entity.h"
#include "macros.h"
#include "config.h"
#include "game/level/content.h"
#include "input.h"

#include "raylib.h"
#include "raymath.h"

namespace Game {

    void Entity::setMoveSpeed(Input* input) {
        if (input->dash && dash_cooldown <= 0.0f && ability_reset)
        {
            move_speed *= 10.0f;
            dash_cooldown = 1.5f;
            ability_reset = false;
        }
        else if (move_speed > base_speed) {
            move_speed -= 75.0f;
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

        if (dash_cooldown > 0.0f)
        {
            dash_cooldown -= dt;
            if (dash_cooldown < 0.0f)
                dash_cooldown = 0.0f;
        }

        if (type == ENTITY_TYPE_MOVING_TILE) {
            Vector2 tmp_pos = Vector2Scale(velocity, dt);
            if (!inBorder(dt)) {
                move_direction.x = move_direction.x * -1;
                move_direction.y = move_direction.y * -1;
            }

            velocity = Vector2Scale(move_direction, move_speed);
            Vector2 delta_pos = Vector2Scale(velocity, dt);
            position = Vector2Add(position, delta_pos);
        }
    }

    void Entity::Control(Input* input,float dt)
    {
        ASSERT(type == ENTITY_TYPE_PLAYER);
        move_direction.x = 0;
        move_direction.x -= input->move[Input::DIRECTION_LEFT] * 2.0f;
        move_direction.x += input->move[Input::DIRECTION_RIGHT] * 2.0f;
        setMoveSpeed(input);
        if (move_direction.x != 0.0f)
            facing = (int)move_direction.x;
        if (input->move[Input::DIRECTION_UP] && on_ground)
            move_direction.y = (float)jump_height * -1.0f;
        if (!on_ground) {
            move_direction.y += 0.125f;
        }
        velocity.x = move_direction.x * move_speed;
        velocity.y = move_direction.y * 25.0f;
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

            default: break;
        }

        Rectangle destination = {};
        destination.x = position.x;
        destination.y = position.y;
        destination.width = size.x;
        destination.height = size.y;

        DrawTexturePro(texture, source, destination, Vector2{}, 0.0f, WHITE);
    }

    bool Entity::collidesWith(Entity ent) {
        if (position.x + size.x > ent.position.x && position.x < ent.position.x + ent.size.x &&
            position.y + size.y > ent.position.y && position.y < ent.position.y + ent.size.y) {
            return 1;
        }
        return 0;
    }
    bool Entity::inBorder(float dt) {
        Vector2 tmp_pos = {};
        tmp_pos.x = position.x + velocity.x * dt;
        tmp_pos.y = position.y + velocity.y * dt;
        if (tmp_pos.x + size.x < endpoints[1].x && tmp_pos.x > endpoints[0].x &&
            tmp_pos.y + size.y < endpoints[1].y && tmp_pos.y > endpoints[0].y) {
            return 1;
        }
        return 0;
    }
}