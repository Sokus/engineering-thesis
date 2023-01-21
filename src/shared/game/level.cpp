#include "level.h"

#include "game/world.h"
#include "macros.h"

namespace Game {

bool load_textures = true;

void SetLoadTextures(bool value)
{
	load_textures = value;
}

void InitPlains(World *world)
{
	world->spawnpoint = Vector2{16.0f, 168.0f};

	// -- INIT ALL ENTITIES --

    world->CreateTile(0.0f, 192.0f, 896.0f, 16.0f, 0,1);
	world->CreateTile(-16.0f, 0.0f, 16.0f, 192.0f, 0, 1);
	world->CreateTile(896.0f, 0.0f, 16.0f, 192.0f, 0, 1);
	world->CreateTile(64.0f, 160.0f, 16.0f, 32.0f, 0, 1);
	world->CreateTile(880.0f, 160.0f, 16.0f, 32.0f, 0, 1);
	world->CreateEnemy(96.0f, 176.0f, 16.0f, 16.0f, 16, 5000);
	world->CreateDamagingTile(80.0f, 176.0f, 800.0f, 16.0f, 0);
	world->CreateInteractive(0.0f, 96.0f, 16.0f, 16.0f, 1);
	world->CreateTile(0.0f, 64.0f, 16.0f, 16.0f,0, 1);
	world->CreateTile(0.0f, 112.0f, 32.0f, 16.0f, 0, 1);

	world->CreateTile(128.0f, 160.0f, 32.0f, 16.0f, 1, 1);
	world->CreateTile(208.0f, 144.0f, 32.0f, 16.0f, 1, 0);

	world->CreateTile(240.0f, 96.0f, 32.0f, 16.0f, 0, 1);
	world->CreateInteractive(256.0f, 80.0f, 16.0f, 16.0f, 2);

	world->CreateMovingTile(16.0f, 48.0f, 32.0f, 16.0f, 2, Vector2{ 30, 0 }, Vector2{ 320.0f, 48.0f }, 0);

	world->CreateDestroyTile(176.0f, 64.0f, 16.0f, 96.0f, 0);
	world->CreateTile(320.0f, 160.0f, 48.0f, 16.0f, 0, 1);
	world->CreateCheckpoint(336.0f, 144.0f, 16.0f, 16.0f);

	world->CreateMovingTile(368.0f, 160.0f, 48.0f, 16.0f, 0, Vector2{ 30, 0 }, Vector2{ 496.0f, 160.0f }, 1);
	world->CreateMovingTile(672.0f, 160.0f, 48.0f, 16.0f, 0, Vector2{ 30, 0 }, Vector2{ 544.0f, 160.0f }, 1);

	world->CreateExit(720.0f, 128.0f, 16.0f, 32.0f);

    // -- INIT BACKGROUND --

	puts("Loading plains!");

	ParallaxLayer hills = {};
	hills.bounds = {-160, 80, 320, 180};
	hills.z = 3;

	ParallaxLayer clouds = {};
	clouds.bounds = {-160, -90, 320, 180};
	clouds.z = 10;
	clouds.scrollSpeed = -100;

	ParallaxLayer sky = {};
	sky.bounds = {-160, -90, 320, 180};
	sky.z = 999;
	sky.emissiveness = 1;

	if (load_textures)
	{
		hills.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/hills.png");
		clouds.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/clouds.png");
		sky.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/sky.png");
	}

	world->parallax_background.AddParallaxLayer(hills);
	world->parallax_background.AddParallaxLayer(clouds);
	world->parallax_background.AddParallaxLayer(sky);
}

void InitLake(World *world) {
    world->spawnpoint = Vector2{ 0.0f, 100.0f };

	world->CreateTile(0.0f, 192.0f, 464.0f, 16.0f, 0,1);
	world->CreateTile(208.0f, 160.0f, 16.0f, 32.0f, 0,1);
	world->CreateTile(400.0f, 160.0f, 16.0f, 32.0f, 0,1);
	world->CreateInteractive(0.0f, 176.0f, 16.0f, 16.0f, 1);
	world->CreateMovingTile(96.0f, 80.0f, 32.0f, 16.0f, 1, Vector2{ 0, 30 }, Vector2{ 96.0f, 160.0f }, 1);
	world->CreateTile(32.0f, 160.0f, 16.0f, 16.0f, 1, 1);
	world->CreateTile(32.0f, 144.0f, 16.0f, 16.0f, 1, 0);
	world->CreateExit(432.0f, 160.0f, 32.0f, 32.0f);
	//world->CreateEnemy(352.0f, 176.0f, 16.0f, 16.0f,27);
	// -- INIT BACKGROUND --

	ParallaxLayer hills = {};
	hills.bounds = {0, 0, 320, 180};
	hills.z = 3;

	ParallaxLayer clouds = {};
	clouds.bounds = {0, 0, 320, 180};
	clouds.z = 10;
	clouds.scrollSpeed = -100;

	ParallaxLayer sky = {};
	sky.bounds = {0, 0, 320, 180};
	sky.z = 999;

	if (load_textures)
	{
		hills.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/hills.png");
		clouds.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/clouds.png");
		sky.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/sky.png");
	}

	world->parallax_background.AddParallaxLayer(hills);
	world->parallax_background.AddParallaxLayer(clouds);
	world->parallax_background.AddParallaxLayer(sky);
}

void InitClouds(World* world) {
	world->spawnpoint = Vector2{ 0.0f, 100.0f };

	world->CreateTile(0.0f, 192.0f, 464.0f, 16.0f, 0,1);
	world->CreateTile(208.0f, 160.0f, 16.0f, 32.0f, 0,1);
	world->CreateTile(400.0f, 160.0f, 16.0f, 32.0f, 0,1);
	//world->CreateTile(224.0f, 96.0f, 64.0f, 16.0f, 0);
	//world->CreateMovingTile(240.0f, 176.0f, 64.0f, 16.0f, 0, Vector2{ 10.0f,0.0f }, Vector2{ 304.0f,176.0f });
	world->CreateMovingTile(240.0f, 176.0f, 64.0f, 16.0f, 0, Vector2{ 10.0f,0.0f }, Vector2{ 240.0f,112.0f },1);
	world->CreateExit(432.0f, 160.0f, 32.0f, 32.0f);
	// -- INIT BACKGROUND --

	ParallaxLayer hills = {};
	hills.bounds = {0, 0, 320, 180};
	hills.z = 3;

	ParallaxLayer clouds = {};
	clouds.bounds = {0, 0, 320, 180};
	clouds.z = 10;
	clouds.scrollSpeed = -100;

	ParallaxLayer sky = {};
	sky.bounds = {0, 0, 320, 180};
	sky.z = 999;

	if (load_textures)
	{
		hills.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/hills.png");
		clouds.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/clouds.png");
		sky.texture = LoadTexture(RESOURCE_PATH "/backgrounds/example_level/sky.png");
	}

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
		case LEVEL_LAKE: InitLake(world); break;
		case LEVEL_CLOUDS: InitClouds(world); break;
        default: break;
    }

	// keep layers sorted by position.z descending to ensure proper draw order
	world->parallax_background.Sort();
}

}