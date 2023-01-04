#ifndef PLAINS_H
#define PLAINS_H
#include "level.h"
enum TextureNum
{
	PLAINTILE,
	PLAINMOVINGTILE,
	PLAINCOLLECTIBLE,
	PLAININTERACTIVE,
	PLAINDMG,
	PLAINDESTROY,
	PLAINCHECKPOINT,
	PLAINEXIT,
	PLAINENEMY
};
extern std::vector<Game::TileData> plainsTiles;
extern std::vector<Game::TileData> plainsMovingTiles;
extern std::vector<Game::TileData> plainsInteractiveTiles;
extern std::vector<Game::TileData> plainsCollectibles;
extern std::vector<Game::TileData> plainsDamagingTiles;
extern std::vector<Game::TileData> plainsDestroyTiles;
extern std::vector<Game::TileData> plainsCheckpoints;
extern std::vector<Game::TileData> plainsExit;
extern std::vector<Game::TileData> plainsEnemies;
extern std::vector<std::string> plainTextures;
namespace Game {
	extern Level plains;
}
#endif