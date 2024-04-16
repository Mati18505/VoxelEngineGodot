#pragma once
#include "Tools/Array3d.h"
#include "VoxelTypes.h"

namespace Voxel {
	class World;
	struct Block;
	struct Biome;
	
	class TerrainGenerator {
	public:
		TerrainGenerator(World& world) : world(world)
		{}
	
		World& world;
	
		void GenerateTerrain(Array3d<Block> &chunkBlocks, Vector2i chunkPosInWorld) const;
		void GenerateStructures(Array3d<Block> &chunkBlocks, Vector2i chunkPosInWorld) const;
		
	private:
		const int maxTerrainHeight = 64;
		const float frequency = 0.03f; // Im mniej tym bardziej płaski teren. //const
	
		float GenerateHeight(float worldX, float worldY) const;
		const Biome &GetBiome(float worldX, float worldY) const;
		BlockID GenerateVoxel(const Biome &biome, int worldZ, int generatedHeight) const;
		bool GenerateTreeProbality(const Biome &biome, float worldX, float worldY) const;
	};
}
