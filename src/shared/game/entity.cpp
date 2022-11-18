#include "entity.h"

namespace Game {
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

        glm::vec2 move_direction = glm::vec2(0.0f, 0.0f);
        move_direction.x -= input->move[Input::Direction::LEFT] * 1.0f * (input->move[Input::Direction::SHIFT] + 1.0f) * 2.0f;
        move_direction.x += input->move[Input::Direction::RIGHT] * 1.0f * (input->move[Input::Direction::SHIFT] + 1.0f) * 2.0f;
        //for (int x =0;x<World)
        //printf("Collide: %d\n",collidesWith(ent));
        if (move_direction.x != 0.0f) facing = (int)move_direction.x;
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
    }

    bool Entity::collidesWith(Entity ent) {
        if (position.x + width * scale > ent.position.x && position.x < ent.position.x + ent.width * ent.scale &&
            position.y + height * scale > ent.position.y && position.y < ent.position.y + ent.height * ent.scale)
            return 1;
        return 0;
    }

}