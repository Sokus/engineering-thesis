#include "level.h"

#include "game/world.h"
#include "macros.h"

namespace Game {

void InitPlains(World *world)
{
	world->spawnpoint = Vector2{0.0f, 100.0f};

	// -- INIT ALL ENTITIES --

    world->CreateTile(0.0f, 192.0f, 464.0f, 16.0f, 0);
    world->CreateTile(64.0f, 160.0f, 16.0f, 32.0f, 0);
    world->CreateTile(112.0f, 64.0f, 96.0f, 16.0f, 0);
    world->CreateTile(208.0f, 0.0f, 16.0f, 160.0f, 0);
    world->CreateTile(208.0f, 160.0f, 16.0f, 32.0f, 1);
    world->CreateTile(336.0f, 176.0f, 16.0f, 16.0f, 0);
    world->CreateTile(400.0f, 176.0f, 16.0f, 16.0f, 0);

    world->CreateMovingTile(96.0f, 80.0f, 32.0f, 16.0f, 0, Vector2{0, 30}, Vector2{96.0f, 160.0f});

    world->CreateInteractive(176.0f, 48.0f, 16.0f, 16.0f, 1);

    world->CreateCollectible(128.0f, 48.0f, 16.0f, 16.0f, 0);
    world->CreateCollectible(96.0f, 144.0f, 16.0f, 16.0f, 0);

    world->CreateDamagingTile(80.0f, 176.0f, 16.0f, 16.0f, 0);

    world->CreateDestroyTile(128.0f, 176.0f, 16.0f, 16.0f, 0);

    world->CreateCheckpoint(144.0f, 176.0f, 16.0f, 16.0f);

    world->CreateExit(432.0f, 160.0f, 32.0f, 32.0f);

    world->CreateEnemy(352.0f, 176.0f, 16.0f, 16.0f);

    // -- INIT BACKGROUND --

	ParallaxLayer hills = {};
	hills.dimensions = {960, 540};
	hills.position = {0, 0, 3};
	hills.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/hills.png");
	hills.natural_scroll_speed = -300;

	ParallaxLayer clouds = {};
	clouds.dimensions = {960, 540};
	clouds.position = {0, 0, 10};
	clouds.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/clouds.png");
	clouds.natural_scroll_speed = -300;

	ParallaxLayer sky = {};
	sky.dimensions = {960, 540};
	sky.position = {0, 0, 999};
	sky.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/sky.png");
	sky.natural_scroll_speed = 0;

	world->parallax_background.AddParallaxLayer(hills);
	world->parallax_background.AddParallaxLayer(clouds);
	world->parallax_background.AddParallaxLayer(sky);
}

void InitLevel(World *world, LevelType type)
{
    ASSERT(type >= 0);
    ASSERT(type != LEVEL_INVALID);

    switch (type)
    {
        case LEVEL_PLAINS: InitPlains(world); break;
        default: break;
    }

	// keep layers sorted by position.z descending to ensure proper draw order
	world->parallax_background.Sort();
}

}