#include "plains.h"

Texture2D plainMovingTile;
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
Game::TileData(   0.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData(  64.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 128.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 192.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 256.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 320.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 384.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 448.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 512.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 576.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 640.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 704.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 768.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 256.0f / 4.0f, 704.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 256.0f / 4.0f, 640.0f / 4.0f, 0, PLAINTILE),

Game::TileData( 448.0f / 4.0f, 256.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 512.0f / 4.0f, 256.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 576.0f / 4.0f, 256.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 640.0f / 4.0f, 256.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 704.0f / 4.0f, 256.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 768.0f / 4.0f, 256.0f / 4.0f, 0, PLAINTILE),

Game::TileData( 832.0f / 4.0f,   0.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 832.0f / 4.0f,  64.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 832.0f / 4.0f, 128.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 832.0f / 4.0f, 192.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 832.0f / 4.0f, 256.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 832.0f / 4.0f, 320.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 832.0f / 4.0f, 384.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 832.0f / 4.0f, 448.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 832.0f / 4.0f, 512.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 832.0f / 4.0f, 576.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 832.0f / 4.0f, 640.0f / 4.0f, 1, PLAINTILE),
Game::TileData( 832.0f / 4.0f, 704.0f / 4.0f, 1, PLAINTILE),
Game::TileData( 832.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 896.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData( 960.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData(1024.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData(1088.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData(1152.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),
Game::TileData(1216.0f / 4.0f, 768.0f / 4.0f, 0, PLAINTILE),

};
glm::vec2 border[2] = { glm::vec2(383.0f / 4.0f, 320.0f / 4.0f),glm::vec2(449.0f / 4.0f, 705.0f / 4.0f) };
std::vector<Game::TileData> plainsMovingTiles = {
	Game::TileData(384.0f / 4.0f, 640.0f / 4.0f, 0, PLAINMOVINGTILE, glm::vec2(0, 100), border)
};
std::vector<Game::TileData> plainsInteractiveTiles = {
	Game::TileData(704.0f / 4.0f, 192.0f / 4.0f, 1, PLAININTERACTIVE)
};
std::vector<Game::TileData> plainsCollectibles = {
	Game::TileData(512.0f / 4.0f, 192.0f / 4.0f, 0, PLAINCOLLECTIBLE),
	Game::TileData(384.0f / 4.0f, 576.0f / 4.0f, 0, PLAINCOLLECTIBLE)
};
std::vector<Game::TileData> plainsDamagingTiles = {
	Game::TileData(320.0f / 4.0f, 704.0f / 4.0f, 0, PLAINDMG)
};
std::vector<Game::TileData> plainsDestroyTiles = {
	Game::TileData(512.0f / 4.0f, 704.0f / 4.0f, 0, PLAINDESTROY)
};
std::vector<Game::TileData> plainsCheckpoints = {
	Game::TileData(576.0f / 4.0f, 704.0f / 4.0f, 0, PLAINCHECKPOINT)
};
std::vector<Game::TileData> plainsExit = {
	Game::TileData(1216.0f / 4.0f, 704.0f / 4.0f, 0, PLAINEXIT)
};

namespace Game {
	Level plains = Level(
		glm::vec2(0.0f, 100.0f),
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

