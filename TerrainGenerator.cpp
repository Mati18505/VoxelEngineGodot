#include "TerrainGenerator.h"
#include <modules/noise/fastnoise_lite.h>
#include "Tools/Math.h"
#include "VoxelTypes.h"
#include "Block.h"
#include "World.h"
#include "Biome.h"
#include "Tools/Profiler.h"

namespace Voxel {
	void TerrainGenerator::GenerateTerrain(Array3d<Block> &chunkBlocks, Vector2i chunkPosInWorld) const
	{
		SM_PROFILE_ZONE;
		const int chunkSize = world.config.chunkSize;
		for (int y = 0; y < chunkSize; y++)
		{
			for (int x = 0; x < chunkSize; x++)
			{
				float worldX = x + chunkPosInWorld.x;
				float worldY = y + chunkPosInWorld.y;
	
				const Biome &biome = GetBiome(worldX, worldY);
				float generatedHeight = GenerateHeight(worldX, worldY);
	
				for (int z = 0; z < world.config.columnHeight * chunkSize; z++) 
				{
					int worldZ = z;
					BlockID blockID = GenerateVoxel(biome, worldZ, generatedHeight);
	
					chunkBlocks.At(Vector3i{ x, y, z }) = Block(blockID);
				}
	
			}
		}
	}
	
	void TerrainGenerator::GenerateStructures(Array3d<Block> &chunkBlocks, Vector2i chunkPosInWorld) const
	{
		SM_PROFILE_ZONE;
		const int chunkSize = world.config.chunkSize;
		const int columnHeight = world.config.columnHeight;
		for (int y = 0; y < chunkSize; y++)
		{
			for (int x = 0; x < chunkSize; x++)
			{
				int worldX = x + chunkPosInWorld.x;
				int worldY = y + chunkPosInWorld.y;
				const Biome &biome = GetBiome(worldX, worldY);
	
				if (!biome.placeMajorFlora)
					continue;
	
				if (!GenerateTreeProbality(biome, worldX, worldY))
					continue;
				//Znajdź blok 1 kratkę nad ziemią
				int treeRootZ = chunkSize * columnHeight - 1;
				while (chunkBlocks.At({ x, y, treeRootZ }).typeID == 0)
				{
					treeRootZ--;
					if (treeRootZ < 0)
						break;
				}
				treeRootZ++;
				if (treeRootZ <= 1)  continue;
				if (treeRootZ + 5 >= chunkSize * columnHeight) continue;
	
				//Jeśli blok pod drzewem nie jest layer1stBlock to drzewo nie powstanie (usuń to jeśli chcesz ciekawe efekty)
				if (chunkBlocks.At({ x, y, treeRootZ - 1 }).typeID != biome.layer1stBlock)
					continue;
	
				//Wygeneruj pień
				for (int z = treeRootZ; z < treeRootZ + 5; z++)
				{
					world.blocksModifications.push({ 3, {worldX, worldY, z} });
				}
	
				//wygeneruj liście
				for (int tx = -4; tx < 4; tx++)
				{
					for (int tz = 0; tz < 4; tz++)
					{
						for (int ty = -4; ty < 4; ty++)
						{
							int totalZ = treeRootZ + 5 + tz;
							if (totalZ >= chunkSize * columnHeight)
								continue;
							int totalX = x + tx;
							int totalY = y + ty;
							int totalWorldX = totalX + chunkPosInWorld.x;
							int totalWorldY = totalY + chunkPosInWorld.y;
	
							world.blocksModifications.push({ 4, {totalWorldX, totalWorldY, totalZ} });
						}
					}
				}
			}
		}
	}
	
	float TerrainGenerator::GenerateHeight(float worldX, float worldY) const {
		FastNoiseLite noise;
		noise.set_noise_type(FastNoiseLite::TYPE_PERLIN);
		noise.set_frequency(frequency);
		noise.set_seed(world.config.seed);
	
		const size_t octavesAmount = 4;
		float octaveFrequencies[octavesAmount] = { 0.03f, 0.01f, 0.02f, 0.005f };
		float octaveAmplitudes[octavesAmount] = { .5f, .9f, .7f, .3f };
		float averageOfOctaveAmplitudes = 0;
		averageOfOctaveAmplitudes = std::accumulate(octaveAmplitudes, octaveAmplitudes + octavesAmount, averageOfOctaveAmplitudes) / (float)octavesAmount;
	
		float height = 0;
		for (size_t i = 0; i < octavesAmount; i++) {
			noise.set_frequency(octaveFrequencies[i]);
			height += octaveAmplitudes[i] * noise.get_noise_2d(worldX, worldY);
		}
		height /= (float)octavesAmount;
	
		height = VoxelMath::MapValue(1, maxTerrainHeight, -averageOfOctaveAmplitudes, averageOfOctaveAmplitudes, height);
	
		return height;
	}
	
	const Biome &TerrainGenerator::GetBiome(float worldX, float worldY) const
	{
		FastNoiseLite noise;
		noise.set_noise_type(FastNoiseLite::TYPE_PERLIN);
		noise.set_frequency(.005f);
		noise.set_seed(world.config.seed);
	
		size_t index = 0;
		if (noise.get_noise_2d(worldX, worldY) > 0.3f)
			index = 1;
	
		return *world.biomes[index];
	}
	
	BlockID TerrainGenerator::GenerateVoxel(const Biome &biome, int worldZ, int generatedHeight) const
	{
		BlockID blockID;
		if (worldZ > generatedHeight)
			blockID = biome.atmosphereBlock;
		else if (worldZ == generatedHeight)
			blockID = biome.layer1stBlock;
		else if (worldZ >= generatedHeight - 5)
			blockID = biome.layer2ndBlock;
		else if (worldZ < generatedHeight)
			blockID = biome.layer3rdBlock;
		return blockID;
	}
	
	bool TerrainGenerator::GenerateTreeProbality(const Biome &biome, float worldX, float worldY) const {
		FastNoiseLite noise;
		noise.set_noise_type(FastNoiseLite::TYPE_SIMPLEX);
		noise.set_seed(world.config.seed);
	
		bool forest = false;
	
		noise.set_frequency(biome.majorFloraZoneScale);
		if (noise.get_noise_2d(worldX, worldY) > biome.majorFloraZoneThreshold)
			forest = true;
	
		bool tree = false;
	
		noise.set_frequency(biome.majorFloraPlacementScale);
		if (forest)
			if (noise.get_noise_2d(worldX, worldY) > biome.majorFloraPlacementThreshold)
				tree = true;
	
		return tree;
	}
}
