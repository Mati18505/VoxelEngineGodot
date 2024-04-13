#pragma once

#include <map>
#include <unordered_map>
#include <thread>
#include <vector>
#include <math.h>
#include <array>
#include <queue>
#include <string>
#include <memory>
#include "vendor/nlohmann/json.hpp"
#include "VoxelTypes.h"

namespace Voxel
{
	struct ChunkPos {
		int x;
		int y;
	
		ChunkPos(int xPos, int yPos) {
			x = xPos;
			y = yPos;
		}
	
		bool operator==(const ChunkPos &other) const {
			return x == other.x && y == other.y;
		}
	};
}

namespace std {
template <>
struct hash<Voxel::ChunkPos> {
	size_t operator()(const Voxel::ChunkPos &key) const {
		size_t val = (size_t)key.x << 32 | key.y;
		return val;
	}
};
} //namespace std

class VoxelNode;

namespace Voxel {
	struct Rectangle;
	struct BlockType;
	class ChunkColumn;
	class TerrainGenerator;
	class Biome;
	using Json = nlohmann::json;
	
	class World {
		Vector2i lastPlayerPos;
		Vector2i currentPlayerPos;
		std::thread buildWorldThread;
		bool isBuildWorldThreadEnd;
	public:
		VoxelNode *gameMode;
		const int worldScale = 1; // constexpr?
		const int chunkSize = 16; 
		const int chunkScaledSize = worldScale * chunkSize;
	
		// config
		int columnHeight = 8;
		int renderDistance = 10;//16
		int seed = 0;
		bool useThreading = false;
		float blockRayCastIncrement = 0.5f;//0.05
		//
	
		std::vector<std::unique_ptr<Biome>> biomes;
		std::unique_ptr<TerrainGenerator> terrainGenerator;
	
		// ChunkPos, Chunk
		std::unordered_map<ChunkPos, ChunkColumn*> chunks;
	
		std::queue<VoxelMod> blocksModifications;
	
		World(VoxelNode* gameMode);
		
		void Start(Json biomesJson, Vector3 playerLocation);
	
		void Update(Vector3 playerLocation);
	
		void GenerateWorld();
		void BuildWorld();
	
		ChunkPos GenerateChunkColumnID(Vector3 chunkPos);
		Vector3 GetChunkColumnPosByBlockWorldPosition(Vector3 blockWorldPosition);
		// Zwraca typ bloku w chunku lub nullptr jeśli poza światem.
		const BlockType *GetBlockTypeInWorld(Vector3 blockWorldPosition);
	
		bool BlockRayCast(Vector3 startPoint, Vector3 direction, Vector3* hitPoint, float range = 100, Vector3* lastHitPoint = nullptr);
		void SetBlock(Vector3 blockPositionInWorld, BlockID blockID);
	
		~World();
	
	private:
		void GenerateBiomeTypes(Json jsonFile);
		void UpdatePlayerPos(Vector3 PlayerLocation);
		bool IsBlockSolid(Vector3 pos);
	};
}
