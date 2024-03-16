#include "TerrainGenerator.h"
//#include <modules/noise/fastnoise_lite.h>
#include "Tools/Math.h"
#include "VoxelTypes.h"
#include "Block.h"
#include "World.h"
#include "Biome.h"

namespace Voxel {
	void TerrainGenerator::GenerateTerrain(Array3d<Block>& chunkBlocks, Vector2i chunkPosInWorld)
	{
		const int chunkSize = world->chunkSize;
		for (int y = 0; y < chunkSize; y++)
		{
			for (int x = 0; x < chunkSize; x++)
			{
				float worldX = x + chunkPosInWorld.x;
				float worldY = y + chunkPosInWorld.y;
	
				Biome& biome = GetBiome(worldX, worldY);
				float generatedHeight = GenerateHeight(worldX, worldY);
	
				for (int z = 0; z < world->columnHeight * chunkSize; z++) 
				{
					int worldZ = z;
					BlockID blockID = GenerateVoxel(biome, worldZ, generatedHeight);
	
					chunkBlocks.At(Vector3i{ x, y, z }) = Block(blockID);
				}
	
			}
		}
	}
	
	void TerrainGenerator::GenerateStructures(Array3d<Block>& chunkBlocks, Vector2i chunkPosInWorld)
	{
		/*const int chunkSize = world->chunkSize;
		const int columnHeight = world->columnHeight;
		for (int y = 0; y < chunkSize; y++)
		{
			for (int x = 0; x < chunkSize; x++)
			{
				int worldX = x + chunkPosInWorld.x;
				int worldY = y + chunkPosInWorld.y;
				Biome& biome = GetBiome(worldX, worldY);
	
				if (!biome.placeMajorFlora)
					continue;
	
				if (!GenerateTreeProbality(biome, worldX, worldY))
					continue;
				//Znajdź blok 1 kratkę nad ziemią
				int treeRootZ = chunkSize * columnHeight - 1;
				while (chunkBlocks.At({ x, y, treeRootZ }).GetBlockTypeID() == 0)
				{
					treeRootZ--;
					if (treeRootZ < 0)
						break;
				}
				treeRootZ++;
				if (treeRootZ <= 1)  continue;
				if (treeRootZ + 5 >= chunkSize * columnHeight) continue;
	
				//Jeśli blok pod drzewem nie jest layer1stBlock to drzewo nie powstanie (usuń to jeśli chcesz ciekawe efekty)
				if (chunkBlocks.At({ x, y, treeRootZ - 1 }).GetBlockTypeID() != biome.layer1stBlock)
					continue;
	
				//Wygeneruj pień
				for (int z = treeRootZ; z < treeRootZ + 5; z++)
				{
					world->blocksModifications.push({ 3, {worldX, worldY, z} });
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
	
							world->blocksModifications.push({ 4, {totalWorldX, totalWorldY, totalZ} });
						}
					}
				}
			}
		}*/
	}
	
	float TerrainGenerator::GenerateHeight(float worldX, float worldY) {
		/*FastNoiseLite noise;
		noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		noise.SetFrequency(frequency);
		noise.SetSeed(world->seed);
	
		const size_t octavesAmount = 4;
		float octaveFrequencies[octavesAmount] = { 0.03f, 0.01f, 0.02f, 0.005f };
		float octaveAmplitudes[octavesAmount] = { .5f, .9f, .7f, .3f };
		float averageOfOctaveAmplitudes = 0;
		averageOfOctaveAmplitudes = std::accumulate(octaveAmplitudes, octaveAmplitudes + octavesAmount, averageOfOctaveAmplitudes) / (float)octavesAmount;
	
		float height = 0;
		for (size_t i = 0; i < octavesAmount; i++) {
			noise.SetFrequency(octaveFrequencies[i]);
			height += octaveAmplitudes[i] * noise.GetNoise(worldX, worldY);
		}
		height /= (float)octavesAmount;
	
		height = VoxelMath::MapValue(1, maxTerrainHeight, -averageOfOctaveAmplitudes, averageOfOctaveAmplitudes, height);
	
		return height;*/
		return 60;
	}
	
	Biome& TerrainGenerator::GetBiome(float worldX, float worldY)
	{
		/*FastNoiseLite noise;
		noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		noise.SetFrequency(.005f);
		noise.SetSeed(world->seed);
	
		size_t index = 0;
		if (noise.GetNoise(worldX, worldY) > 0.3f)
			index = 1;
	
		return *world->biomes[index];*/
		return *world->biomes[0];
	}
	
	BlockID TerrainGenerator::GenerateVoxel(Biome& biome, int worldZ, int generatedHeight)
	{
		/*BlockID blockID;
		if (worldZ > generatedHeight)
			blockID = biome.atmosphereBlock;
		else if (worldZ == generatedHeight)
			blockID = biome.layer1stBlock;
		else if (worldZ >= generatedHeight - 5)
			blockID = biome.layer2ndBlock;
		else if (worldZ < generatedHeight)
			blockID = biome.layer3rdBlock;
		return blockID;*/
		return 1;
	}
	
	bool TerrainGenerator::GenerateTreeProbality(Biome& biome, float worldX, float worldY) {
		
	
		/*FastNoiseLite noise;
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noise.SetSeed(world->seed);
	
		bool forest = false;
	
		noise.SetFrequency(biome.majorFloraZoneScale);
		if (noise.GetNoise(worldX, worldY) > biome.majorFloraZoneThreshold)
			forest = true;
	
		bool tree = false;
	
		noise.SetFrequency(biome.majorFloraPlacementScale);
		if (forest)
			if (noise.GetNoise(worldX, worldY) > biome.majorFloraPlacementThreshold)
				tree = true;
	
		return tree;*/
		return false;
	}
}
