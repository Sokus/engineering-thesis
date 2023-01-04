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
        if (input->dash && input->cooldown > 90 && playerData.ability_reset) {
            move_speed *= 10;
            input->cooldown = 0;
            playerData.ability_reset = 0;
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

        if (type == ENTITY_TYPE_MOVING_TILE) {
            glm::vec2 tmp_pos = rF.velocity * dt;
            if (!inBorder(dt)) {
                move_direction.x = move_direction.x * -1;
                move_direction.y = move_direction.y * -1;
            }
            rF.velocity = move_direction * move_speed;

            rF.position += rF.velocity * dt;
        }
    }

    void Entity::Control(Input* input,float dt)
    {
        ASSERT(type == ENTITY_TYPE_PLAYER);
        move_direction.x = 0;
        move_direction.x -= input->move[Input::Direction::LEFT] * 2.0f;
        move_direction.x += input->move[Input::Direction::RIGHT] * 2.0f;
        setMoveSpeed(input);
        if (move_direction.x != 0.0f) playerData.facing = (int)move_direction.x;
        if (input->move[Input::Direction::UP] && playerData.onGround)
            move_direction.y = (float)playerData.jumpHeight * -1.0f;
        if (!playerData.onGround) {
            move_direction.y += 0.125f;
        }
        rF.velocity.x = move_direction.x * move_speed;
        rF.velocity.y = move_direction.y * 25.0f;
    }
    void Entity::MoveX(float dt) {
        ASSERT(type == ENTITY_TYPE_PLAYER);
        rF.position.x += rF.velocity.x * dt;
    }
    void Entity::MoveY(float dt) {
        ASSERT(type == ENTITY_TYPE_PLAYER);
        rF.position.y += rF.velocity.y * dt;
    }

    void Entity::Draw()
    {
        if (hidden)
            return;

        Rectangle source = Rectangle{ 0.0f, 0.0f, (float)width, (float)height };

        if (num_frames)
        {
            source.x = (float)(current_frame * width);
        }

        switch (type)
        {
            case ENTITY_TYPE_PLAYER:
            {
                source.y = (ABSF(rF.velocity.x) <= 0.001f ? 0.0f : (float)height);
                source.width = (float)(playerData.facing >= 0 ? width : -width);
            } break;

            case ENTITY_TYPE_INTERACTIVE:
            {
                if (enabled)
                    source.x = (float)width;
            } break;

            default: break;
        }

        Rectangle destination = {};
        destination.x = rF.position.x;
        destination.y = rF.position.y;
        destination.width = (float)width;
        destination.height = (float)height;

        DrawTexturePro(texture, source, destination, Vector2{}, 0.0f, WHITE);
    }

    bool Entity::collidesWith(Entity ent) {
        if (rF.position.x + width > ent.rF.position.x && rF.position.x < ent.rF.position.x + ent.width &&
            rF.position.y + height > ent.rF.position.y && rF.position.y < ent.rF.position.y + ent.height) {
            return 1;
        }
        return 0;
    }
    bool Entity::inBorder(float dt) {
        glm::vec2 tmp_pos = rF.position + (rF.velocity * dt);
        if (tmp_pos.x + width < border[1].x && tmp_pos.x > border[0].x &&
            tmp_pos.y + height < border[1].y && tmp_pos.y > border[0].y) {
            return 1;
        }
        return 0;
    }
}