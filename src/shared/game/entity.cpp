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

        if (type == ENTITY_TYPE_PLAYER)
        {

            if (animation_frame_time >= max_animation_frame_time)
            {
                animation_frame++;
                animation_frame_time = 0.0f;
            }

            animation_frame_time += dt;
        }
        if (type == ENTITY_TYPE_INTERACTIVE) {
            if (enabled) {
                animation_frame = 1;
            }
            else {
                animation_frame = 0;
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
        // if (type == BULLET) {
        //     rF.Update(dt);
        //     rF.velocity.y += bulletData.gravity * dt;
        //     rF.velocity *= 1 + bulletData.ln1MinusDragCoefficient * dt;
        //     bulletData.lifetime += dt;
        // }

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
        if (type == ENTITY_TYPE_PLAYER)
        {
            width = 16;
            height = 24;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 4 * width);
            source.y = (ABSF(rF.velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(playerData.facing >= 0 ? width : -width);
            Rectangle dest = {
                rF.position.x,
                rF.position.y,
                (float)(width),
                (float)(height)
            };

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == ENTITY_TYPE_TILE && visible) {
            width = 16;
            height = 16;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 4 * width);
            source.y = (ABSF(rF.velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(width);
            Rectangle dest = {
                rF.position.x,
                rF.position.y,
                (float)(width),
                (float)(height)
            };

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == ENTITY_TYPE_INTERACTIVE && visible) {
            width = 16;
            height = 16;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 2 * width);
            source.y = (ABSF(rF.velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(width);
            Rectangle dest = {
                rF.position.x,
                rF.position.y,
                (float)(width),
                (float)(height)
            };

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == ENTITY_TYPE_MOVING_TILE && visible) {
            width = 16;
            height = 16;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 2 * width);
            source.y = (ABSF(rF.velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(width);
            Rectangle dest = {
                rF.position.x,
                rF.position.y,
                (float)(width),
                (float)(height)
            };
            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == ENTITY_TYPE_COLLECTIBLE && visible) {
            width = 16;
            height = 16;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 4 * width);
            source.y = (ABSF(rF.velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(width);
            Rectangle dest = {
                rF.position.x,
                rF.position.y,
                (float)(width),
                (float)(height)
            };

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == ENTITY_TYPE_DAMAGING_TILE && visible) {
            width = 16;
            height = 16;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 4 * width);
            source.y = (ABSF(rF.velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(width);
            Rectangle dest = {
                rF.position.x,
                rF.position.y,
                (float)(width),
                (float)(height)
            };

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        /*
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

                dest.x = rF.position.x;
                dest.y = rF.position.y;
                dest.width = bulletData.visibleSize.x * sizeMultiplier;
                dest.height = bulletData.visibleSize.y * sizeMultiplier;

                DrawTexturePro(
                    texture,
                    source, dest,
                    { bulletData.visibleSize.x * sizeMultiplier / 2, bulletData.visibleSize.y * sizeMultiplier / 2 },
                    rF.rotation,
                    { 255,255,255,static_cast<unsigned char>(255 * alphaMultiplier) }
                );
            }
        }
        */
        if (type == ENTITY_TYPE_DESTRUCTIBLE_TILE && visible) {
            width = 16;
            height = 16;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 4 * width);
            source.y = (ABSF(rF.velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(width);
            Rectangle dest = {
                rF.position.x,
                rF.position.y,
                (float)(width),
                (float)(height)
            };

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == ENTITY_TYPE_CHECKPOINT && visible) {
            width = 16;
            height = 16;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 2 * width);
            source.y = (ABSF(rF.velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(width);
            Rectangle dest = {
                rF.position.x,
                rF.position.y,
                (float)(width),
                (float)(height)
            };

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
        if (type == ENTITY_TYPE_EXIT && visible) {
            width = 16;
            height = 16;
            Rectangle source = Rectangle{ 0.0f, 0.0f, 0.0f, (float)height };
            source.x = (float)(animation_frame % 2 * width);
            source.y = (ABSF(rF.velocity.x) <= 0.001f ? 0.0f : (float)height);
            source.width = (float)(width);
            Rectangle dest = {
                rF.position.x,
                rF.position.y,
                (float)(width),
                (float)(height)
            };

            DrawTexturePro(texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 255, 255, 255, 255 });
        }
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