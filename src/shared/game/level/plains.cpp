#include "plains.h"

namespace Game {

	Texture2D plainMovingTile;
	std::vector<std::string> plainTextures = {
		RESOURCE_PATH "/tile.png",
		RESOURCE_PATH "/movingtile.png",
		RESOURCE_PATH "/collectible.png",
		RESOURCE_PATH "/interactive.png",
		RESOURCE_PATH "/dmgtile.png",
		RESOURCE_PATH "/destroytile.png",
	};
	std::vector<TileData> plainsTiles = {
	TileData(0.0f, 768.0f, 0,PLAINTILE),
	TileData(64.0f, 768.0f, 0,PLAINTILE),
	TileData(128.0f, 768.0f, 0,PLAINTILE),
	TileData(192.0f, 768.0f, 0,PLAINTILE),
	TileData(256.0f, 768.0f, 0,PLAINTILE),
	TileData(320.0f, 768.0f, 0,PLAINTILE),
	TileData(384.0f, 768.0f, 0,PLAINTILE),
	TileData(448.0f, 768.0f, 0,PLAINTILE),
	TileData(512.0f, 768.0f, 0,PLAINTILE),
	TileData(576.0f, 768.0f, 0,PLAINTILE),
	TileData(640.0f, 768.0f, 0,PLAINTILE),
	TileData(704.0f, 768.0f, 0,PLAINTILE),
	TileData(768.0f, 768.0f, 0,PLAINTILE),

	TileData(256.0f, 704.0f, 0,PLAINTILE),
	TileData(256.0f, 640.0f, 0,PLAINTILE),

	TileData(448.0f, 256.0f, 0,PLAINTILE),
	TileData(512.0f, 256.0f, 0,PLAINTILE),
	TileData(576.0f, 256.0f, 0,PLAINTILE),
	TileData(640.0f, 256.0f, 0,PLAINTILE),
	TileData(704.0f, 256.0f, 0,PLAINTILE),
	TileData(768.0f, 256.0f, 0,PLAINTILE),

	TileData(832.0f, 0.0f, 0,PLAINTILE),
	TileData(832.0f, 64.0f, 0,PLAINTILE),
	TileData(832.0f, 128.0f, 0,PLAINTILE),
	TileData(832.0f, 192.0f, 0,PLAINTILE),
	TileData(832.0f, 256.0f, 0,PLAINTILE),
	TileData(832.0f, 320.0f, 0,PLAINTILE),
	TileData(832.0f, 384.0f, 0,PLAINTILE),
	TileData(832.0f, 448.0f, 0,PLAINTILE),
	TileData(832.0f, 512.0f, 0,PLAINTILE),
	TileData(832.0f, 576.0f, 0,PLAINTILE),
	TileData(832.0f, 640.0f, 1,PLAINTILE),
	TileData(832.0f, 704.0f, 1,PLAINTILE),
	TileData(832.0f, 768.0f, 0,PLAINTILE),

	};
	glm::vec2 border[2] = { glm::vec2(383.0f, 320.0f),glm::vec2(449.0f,705.0f) };
	std::vector<TileData> plainsMovingTiles = {
		TileData(384.0f, 640.0f, 0,PLAINMOVINGTILE, glm::vec2(0, 100),border)
	};
	std::vector<TileData> plainsInteractiveTiles = {
		TileData(704.0f, 192.0f, 1, PLAININTERACTIVE)
	};
	std::vector<TileData> plainsCollectibles = {
		TileData(512.0f, 192.0f, 0,PLAINCOLLECTIBLE),
		TileData(384.0f, 576.0f, 0,PLAINCOLLECTIBLE)
	};
	std::vector<TileData> plainsDamagingTiles = {
		TileData(320.0f, 704.0f, 0,PLAINDMG)
	};
	std::vector<TileData> plainsDestroyTiles = {
		TileData(512.0f, 704.0f, 0,PLAINDESTROY)
	};
}

