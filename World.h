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
#include "GameMode.h"

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

namespace Voxel {
	struct Rectangle;
	struct BlockType;
	class ChunkColumn;
	class TerrainGenerator;
	class Biome;
	using Json = nlohmann::json;
	
	class World {
		Vector2i lastPlayerPos = Vector2i(-1, 0);
		Vector2i currentPlayerPos = Vector2i(0, 0);
		std::thread buildWorldThread;
		bool isBuildWorldThreadEnd = false;

	public:
		const GameMode::Config& config;
		GameMode &gameMode;

		std::vector<std::unique_ptr<Biome>> biomes;
		std::unique_ptr<TerrainGenerator> terrainGenerator;

		// ChunkPos, Chunk
		std::unordered_map<ChunkPos, ChunkColumn *> chunks;

		std::queue<VoxelMod> blocksModifications;

		World(GameMode &gameMode, std::vector<std::unique_ptr<Biome>> biomes);

		void Start(Vector3 playerLocation);
		void Update(Vector3 playerLocation);

		void GenerateWorld();
		void BuildWorld();

		ChunkPos GenerateChunkColumnID(Vector3 chunkPos);
		Vector3 GetChunkColumnPosByBlockWorldPosition(Vector3 blockWorldPosition);
		// Zwraca typ bloku w chunku lub nullptr jeśli poza światem.
		const BlockType* GetBlockTypeInWorld(Vector3 blockWorldPosition);

		bool BlockRayCast(Vector3 startPoint, Vector3 direction, Vector3 *hitPoint, float range = 100, Vector3 *lastHitPoint = nullptr);
		void SetBlock(Vector3 blockPositionInWorld, BlockID blockID);

		~World();

	private:
		void UpdatePlayerPos(Vector3 PlayerLocation);
		bool IsBlockSolid(Vector3 pos);
	};
}
