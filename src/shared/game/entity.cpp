#include "entity.h"
#include <macros.h>
#include <raylib.h>
#include <config.h>
#include <game/level/content.h>

namespace Game {

    void BulletData::SetDrag(float dragCoefficient) {
        ln1MinusDragCoefficient = log(1 - dragCoefficient);
    }
    float interpolate(float progress, const std::vector<float>& keyframes) {

        if (keyframes.size() == 1)
            return keyframes.front();

        if (progress < 0)
            return keyframes.front();

        if (progress > 1)
            return keyframes.back();

        int intPart = progress * (keyframes.size() - 1);
        float realPart = progress * (keyframes.size() - 1) - intPart;

        if (intPart >= keyframes.size() - 1) {
            intPart = keyframes.size() - 2;
            realPart = 1;
        }

        return glm::mix(keyframes[intPart], keyframes[intPart + 1], realPart);
    }
    void Entity::setMoveSpeed(Input* input) {
        if (input->sprint && input->cooldown > 90) {
            move_speed *= 10;
            input->cooldown = 0;
        }
        else if (move_speed > 200) {
            move_speed -= 250.0f;
        }
        else {
            move_speed = 200.0f;
        }
    }

	void Entity::Update(float dt)
    {

        if (type == PLAYER)
        {

            if (animation_frame_time >= max_animation_frame_time)
            {
                animation_frame++;
                animation_frame_time = 0.0f;
            }

            animation_frame_time += dt;
        }
        if (type == INTERACTIVE) {
            if (active) {
                animation_frame = 1;
            }
            else {
                animation_frame = 0;
            }
        }
        if (type == MOVING_TILE) {
            glm::vec2 tmp_pos = velocity * dt;
            if (!inBorder(dt)) {
                velocity = glm::vec2(0.0f, 0.0f);
                move_direction = move_direction * glm::vec2(-1, -1);
            }
            else {
                velocity = move_direction * move_speed;
            }
            position += velocity * dt;
        }
        if (type == BULLET) {
            bulletData.referenceFrame.Update(dt);
            bulletData.referenceFrame.velocity.y += bulletData.gravity * dt;
            bulletData.referenceFrame.velocity *= 1 + bulletData.ln1MinusDragCoefficient * dt;
            bulletData.lifetime += dt;
        }

    }

    void Entity::Control(Input* input,float dt)
    {
        ASSERT(type == PLAYER);
        move_direction.x = 0;
        move_direction.x -= input->move[Input::Direction::LEFT] * 2.0f;
        move_direction.x += input->move[Input::Direction::RIGHT] * 2.0f;
        setMoveSpeed(input);
        if (move_direction.x != 0.0f) facing = (int)move_direction.x;
        if (input->move[Input::Direction::UP] && onGround)
            move_direction.y = jumpHeight*-1;
        if (!onGround) {
            move_direction.y += 0.5;
        }
        velocity.x = move_direction.x * move_speed;
        velocity.y = move_direction.y * 100.0f;
    }
    void Entity::MoveX(float dt) {
        ASSERT(type == PLAYER);
        position.x += velocity.x * dt;
    }
    void Entity::MoveY(float dt) {
        ASSERT(type == PLAYER);
        position.y += velocity.y * dt;
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

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == TILE && active) {
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

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == INTERACTIVE) {
            width = 16;
            height = 16;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 2 * width);
            source.y = (ABSF(velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(facing >= 0 ? width : -width);
            Rectangle dest = {
                position.x,
                position.y,
                (float)(scale * width),
                (float)(scale * height)
            };

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == MOVING_TILE && active) {
            width = 16;
            height = 16;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 2 * width);
            source.y = (ABSF(velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(facing >= 0 ? width : -width);
            Rectangle dest = {
                position.x,
                position.y,
                (float)(scale * width),
                (float)(scale * height)
            };
            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == COLLECTIBLE && active) {
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

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == DAMAGING_TILE && active) {
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

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == BULLET) {
            if (bulletData.lifetime > bulletData.maxLifetime) {
                type = NONE;
            }
            else {
                const int totalAnimationFrames = bulletData.animationFrames.x * bulletData.animationFrames.y;
                const int frameNo = static_cast<int>(bulletData.lifetime / bulletData.animationLength * totalAnimationFrames) % totalAnimationFrames;
                const glm::vec2 frameSize = glm::vec2(texture.width, texture.height) / glm::vec2(bulletData.animationFrames);
                bulletData.SetDrag(0.5);
                // 0 when spawned, 1 when at max lifetime
                const float lifetimeProgress = bulletData.lifetime / bulletData.maxLifetime;
                const float sizeMultiplier = interpolate(lifetimeProgress, bulletData.sizeKeyframes);
                const float alphaMultiplier = interpolate(lifetimeProgress, bulletData.alphaKeyframes);

                Rectangle source, dest;

                source.x = frameSize.x * (frameNo % bulletData.animationFrames.x);
                source.y = frameSize.y * (frameNo / bulletData.animationFrames.x);;
                source.width = frameSize.x;
                source.height = frameSize.y;

                dest.x = bulletData.referenceFrame.position.x;
                dest.y = bulletData.referenceFrame.position.y;
                dest.width = bulletData.visibleSize.x * sizeMultiplier;
                dest.height = bulletData.visibleSize.y * sizeMultiplier;

                DrawTexturePro(
                    texture,
                    source, dest,
                    { bulletData.visibleSize.x * sizeMultiplier / 2, bulletData.visibleSize.y * sizeMultiplier / 2 },
                    bulletData.referenceFrame.rotation,
                    { 255,255,255,static_cast<unsigned char>(255 * alphaMultiplier) }
                );
            }
        }
        if (type == DESTROY_TILE && active) {
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

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
    }

    bool Entity::collidesWith(Entity ent) {
        if (position.x + width * scale > ent.position.x && position.x < ent.position.x + ent.width * ent.scale &&
            position.y + height * scale > ent.position.y && position.y < ent.position.y + ent.height * ent.scale) {
            return 1;
        }
        return 0;
    }
    bool Entity::inBorder(float dt) {
        glm::vec2 tmp_pos = glm::vec2(0.0f, 0.0f);
        tmp_pos = position + (velocity * dt);
        if (tmp_pos.x + width * scale < border[1].x && tmp_pos.x > border[0].x &&
            tmp_pos.y + height * scale < border[1].y && tmp_pos.y > border[0].y) {
            return 1;
        }
        return 0;
    }
}