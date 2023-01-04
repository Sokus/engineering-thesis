#include "plains.h"

std::vector<std::string> plainTextures = {
	RESOURCE_PATH "/tile.png",
	RESOURCE_PATH "/movingtile.png",
	RESOURCE_PATH "/collectible.png",
	RESOURCE_PATH "/interactive.png",
	RESOURCE_PATH "/dmgtile.png",
	RESOURCE_PATH "/destroytile.png",
	RESOURCE_PATH "/checkpoint.png",
	RESOURCE_PATH "/exit.png"
};

std::vector<Game::TileData> plainsTiles = {
Game::TileData(   0.0f, 192.0f,352.0f,16.0f, 0, PLAINTILE),

Game::TileData( 64.0f, 160.0f,16.0f,32.0f, 0, PLAINTILE),

Game::TileData( 112.0f, 64.0f,96.0f,16.0f, 0, PLAINTILE),

Game::TileData( 208.0f, 0.0f,16.0f,160.0f, 0, PLAINTILE),

Game::TileData( 208.0f, 160.0f,16.0f,32.0f, 1, PLAINTILE),

};
Vector2 border[2] = { Vector2{95.0f, 80.0f}, Vector2{129.0f, 177.0f} };
std::vector<Game::TileData> plainsMovingTiles = {
	Game::TileData(96.0f, 160.0f,32.0f,16.0f, 0, PLAINMOVINGTILE, Vector2{0, 30}, border)
};
std::vector<Game::TileData> plainsInteractiveTiles = {
	Game::TileData(176.0f, 48.0f,16.0f,16.0f, 1, PLAININTERACTIVE)
};
std::vector<Game::TileData> plainsCollectibles = {
	Game::TileData(128.0f, 48.0f,16.0f,16.0f, 0, PLAINCOLLECTIBLE),
	Game::TileData(96.0f, 144.0f,16.0f,16.0f, 0, PLAINCOLLECTIBLE)
};
std::vector<Game::TileData> plainsDamagingTiles = {
	Game::TileData(80.0f, 176.0f,16.0f,16.0f, 0, PLAINDMG)
};
std::vector<Game::TileData> plainsDestroyTiles = {
	Game::TileData(128.0f, 176.0f,16.0f,16.0f, 0, PLAINDESTROY)
};
std::vector<Game::TileData> plainsCheckpoints = {
	Game::TileData(144.0f, 176.0f,16.0f,16.0f, 0, PLAINCHECKPOINT)
};
std::vector<Game::TileData> plainsExit = {
	Game::TileData(320.0f, 160.0f,32.0f,32.0f, 0, PLAINEXIT)
};

namespace Game {
	Level plains = Level(
		Vector2{0.0f, 100.0f},
		plainsTiles,
		plainsMovingTiles,
		plainsInteractiveTiles,
		plainsCollectibles,
		plainsDamagingTiles,
		plainsDestroyTiles,
		plainsCheckpoints,
		plainsExit,
		plainTextures);
}

