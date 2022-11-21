#include "entity.h"
#include <macros.h>
#include <raylib.h>
#include <config.h>

namespace Game {


    Texture2D character;
    Texture2D tile;
    Texture2D interactive;



    void LoadTextures()
    {
        character = LoadTexture(RESOURCE_PATH "/character.png");
        tile = LoadTexture(RESOURCE_PATH "/tile.png");
        interactive = LoadTexture(RESOURCE_PATH "/interactive.png");
    }

	void Entity::Update(float dt)
    {
        position += velocity * dt;

        if (type == PLAYER)
        {

            if (animation_frame_time >= max_animation_frame_time)
            {
                animation_frame++;
                animation_frame_time = 0.0f;
            }

            animation_frame_time += dt;
        }
    }

    void Entity::Control(Input* input)
    {
        ASSERT(type == PLAYER);
        move_direction.x = 0;
        move_direction.x -= input->move[Input::Direction::LEFT] * 1.0f * (input->move[Input::Direction::SHIFT] + 1.0f) * 2.0f;
        move_direction.x += input->move[Input::Direction::RIGHT] * 1.0f * (input->move[Input::Direction::SHIFT] + 1.0f) * 2.0f;
        if (move_direction.x != 0.0f) facing = (int)move_direction.x;
        if (input->move[Input::Direction::UP] && onGround)
            move_direction.y = jumpHeight*-1;
        if (!onGround) {
            move_direction.y += 1;
        }
        velocity = move_direction * move_speed;

    }

    void Entity::Draw()
    {
        if (type == PLAYER)
        {
            width = 16;
            height = 24;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 4 * width);
            source.y = (ABSF(velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(facing >= 0 ? width : -width);
            Rectangle dest = {
                position.x,
                position.y,
                (float)(scale * width),
                (float)(scale * height)
            };

            DrawTexturePro(character, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == TILE) {
            width = 16;
            height = 16;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 4 * width);
            source.y = (ABSF(velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(facing >= 0 ? width : -width);
            Rectangle dest = {
                position.x,
                position.y,
                (float)(scale * width),
                (float)(scale * height)
            };

            DrawTexturePro(tile, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == INTERACTIVE) {
            width = 16;
            height = 16;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 4 * width);
            source.y = (ABSF(velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(facing >= 0 ? width : -width);
            Rectangle dest = {
                position.x,
                position.y,
                (float)(scale * width),
                (float)(scale * height)
            };

            DrawTexturePro(interactive, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
    }

    bool Entity::collidesWithX(Entity ent,float dt) {
        glm::vec2 tmp_pos = glm::vec2(0.0f, 0.0f);
        glm::vec2 velo = glm::vec2(velocity.x,0.0f);
        tmp_pos = position + (velo * dt);
        if (tmp_pos.x + width * scale > ent.position.x && tmp_pos.x < ent.position.x + ent.width * ent.scale &&
            tmp_pos.y + height * scale > ent.position.y && tmp_pos.y < ent.position.y + ent.height * ent.scale) {
            return 1;
        }
        return 0;
    }
    bool Entity::collidesWithY(Entity ent, float dt) {
        glm::vec2 tmp_pos = glm::vec2(0.0f, 0.0f);
        glm::vec2 velo = glm::vec2(0.0f,velocity.y);
        tmp_pos = position + (velo * dt);
        if (tmp_pos.x + width * scale > ent.position.x && tmp_pos.x < ent.position.x + ent.width * ent.scale &&
            tmp_pos.y + height * scale > ent.position.y && tmp_pos.y < ent.position.y + ent.height * ent.scale) {
            return 1;
        }
        return 0;
    }

}