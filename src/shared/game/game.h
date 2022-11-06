#ifndef GAME_H
#define GAME_H

#include "input.h"

#include "glm/glm.hpp"

namespace Game {

enum EntityType
{
    NONE,
    PLAYER,
    ENEMY,
};

enum PlayerVariant
{
    PLAYER_RED,
    PLAYER_GREEN,
    PLAYER_BLUE,
    PLAYER_WHITE,
};

struct Entity
{
    // SHARED STATE
    EntityType type;

    glm::vec2 position;
    glm::vec2 velocity;

    int facing;

    float move_speed;
    int variant;

    // CLIENT STATE

    // TODO: more robust animation system
    unsigned int animation_frame;
    float animation_frame_time;
    float max_animation_frame_time;

    void Update(float dt);
    void Draw();

    void Control(Input *input);
};

class World
{
private:
    static const int entity_count = 256;
    Entity entities[entity_count] = {};

public:
    void Update(float dt);
    void Draw();

    Entity *GetNewEntity();
    Entity *CreatePlayer(float pos_x, float pos_y);
};

} // namespace Game

#endif // GAME_H