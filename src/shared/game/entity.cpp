#include "entity.h"
#include "macros.h"
#include "config.h"
#include "game/level/content.h"
#include "input.h"

#include "raylib.h"

namespace Game {

    // void BulletData::SetDrag(float dragCoefficient) {
    //     ln1MinusDragCoefficient = log(1 - dragCoefficient);
    // }
    float interpolate(float progress, const std::vector<float>& keyframes) {

        if (keyframes.size() == 1)
            return keyframes.front();

        if (progress < 0)
            return keyframes.front();

        if (progress > 1)
            return keyframes.back();

        int intPart = (int)(progress * (float)(keyframes.size() - 1));
        float realPart = progress * (keyframes.size() - 1) - intPart;

        if (intPart >= keyframes.size() - 1) {
            intPart = (int)keyframes.size() - 2;
            realPart = 1;
        }

        return glm::mix(keyframes[intPart], keyframes[intPart + 1], realPart);
    }
    void Entity::setMoveSpeed(Input* input) {
        if (input->dash && input->cooldown > 90 && ability_reset) {
            move_speed *= 10;
            input->cooldown = 0;
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
                frame_time = 0.0f;
            }
        }

        if (time_until_state_change_allowed > 0.0f)
        {
            time_until_state_change_allowed -= dt;
            if (time_until_state_change_allowed < 0.0f)
                time_until_state_change_allowed = 0.0f;
        }

        if (type == ENTITY_TYPE_MOVING_TILE) {
            glm::vec2 tmp_pos = velocity * dt;
            if (!inBorder(dt)) {
                move_direction.x = move_direction.x * -1;
                move_direction.y = move_direction.y * -1;
            }
            velocity = move_direction * move_speed;

            position += velocity * dt;
        }
    }

    void Entity::Control(Input* input,float dt)
    {
        ASSERT(type == ENTITY_TYPE_PLAYER);
        move_direction.x = 0;
        move_direction.x -= input->move[Input::Direction::LEFT] * 2.0f;
        move_direction.x += input->move[Input::Direction::RIGHT] * 2.0f;
        setMoveSpeed(input);
        if (move_direction.x != 0.0f)
            facing = (int)move_direction.x;
        if (input->move[Input::Direction::UP] && on_ground)
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
        glm::vec2 tmp_pos = position + (velocity * dt);
        if (tmp_pos.x + size.x < endpoints[1].x && tmp_pos.x > endpoints[0].x &&
            tmp_pos.y + size.y < endpoints[1].y && tmp_pos.y > endpoints[0].y) {
            return 1;
        }
        return 0;
    }
}