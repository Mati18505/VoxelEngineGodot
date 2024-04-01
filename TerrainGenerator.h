#pragma once
#include "Tools/Array3d.h"
#include "VoxelTypes.h"

namespace Voxel {
	class World;
	struct Block;
	class Biome;
	
	class TerrainGenerator {
	public:
		TerrainGenerator(World* world) : world(world)
		{}
	
		World* world;
	
		void GenerateTerrain(Array3d<Block> &chunkBlocks, Vector2i chunkPosInWorld);
		void GenerateStructures(Array3d<Block> &chunkBlocks, Vector2i chunkPosInWorld);
		
	private:
		const int maxTerrainHeight = 64;
		const float frequency = 0.03f; // Im mniej tym bardziej płaski teren. //const
	
		float GenerateHeight(float worldX, float worldY);
		Biome& GetBiome(float worldX, float worldY);
		BlockID GenerateVoxel(Biome& biome, int worldZ, int generatedHeight);
		bool GenerateTreeProbality(Biome& biome, float worldX, float worldY);
	};
}
