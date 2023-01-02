#ifndef PLAINS_H
#define PLAINS_H
#include "level.h"
namespace Game {
	enum TextureNum
	{
		PLAINTILE,
		PLAINMOVINGTILE,
		PLAINCOLLECTIBLE,
		PLAININTERACTIVE,
		PLAINDMG
	};
	extern std::vector<TileData> plainsTiles;
	extern std::vector<TileData> plainsMovingTiles;
	extern std::vector<TileData> plainsInteractiveTiles;
	extern std::vector<TileData> plainsCollectibles;
	extern std::vector<TileData> plainsDamagingTiles;
	extern std::vector<std::string> plainTextures;
}
#endif