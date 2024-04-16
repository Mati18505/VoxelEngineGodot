#pragma once

#include "GameMode.h"
#include "vendor/nlohmann/json.hpp"
#include "VoxelTypes.h"
#include <unordered_map>
#include <thread>
#include <vector>
#include <queue>
#include <memory>

namespace Voxel
{
	struct ChunkPos {
		int x;
		int y;
	
		ChunkPos(int xPos, int yPos) :
			x(xPos), y(yPos)
		{}
	
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
	struct BlockType;
	class ChunkColumn;
	class TerrainGenerator;
	struct Biome;
	using Json = nlohmann::json;
	
	class World {
	public:
		World(GameMode &gameMode, std::vector<std::unique_ptr<const Biome>> biomes);
		~World();

		void Start(const Vector3 &playerLocation);
		void Update(const Vector3 &playerLocation);

		void SetBlock(const Vector3 &blockPositionInWorld, BlockID blockID);

		ChunkPos GenerateChunkColumnID(const Vector3 &chunkPos) const;
		Vector3 GetChunkColumnPosByBlockWorldPosition(const Vector3 &blockWorldPosition) const;
		// Zwraca typ bloku w chunku lub nullptr jeśli poza światem.
		const BlockType *GetBlockTypeInWorld(const Vector3 &blockWorldPosition) const;

		struct RayCastResult {
			bool hitted = false;
			Vector3 hitPoint{};
			Vector3 previousHitPoint{};

			operator bool() { return hitted; }
		};
		RayCastResult BlockRayCast(Vector3 startPoint, const Vector3 &direction, float range = 100) const;

		GameMode &gameMode;
		std::unordered_map<ChunkPos, std::shared_ptr<ChunkColumn>> chunks;
		std::queue<VoxelMod> blocksModifications;
		std::unique_ptr<TerrainGenerator> terrainGenerator;
		const GameMode::Config &config;
		std::vector<std::unique_ptr<const Biome>> biomes;

	private:
		void GenerateWorld();
		void BuildWorld();

		void UpdatePlayerPos(const Vector3 &PlayerLocation);
		bool IsBlockSolid(const Vector3 &pos) const;

		Vector2i lastPlayerPos = Vector2i(-1, 0);
		Vector2i currentPlayerPos = Vector2i(0, 0);
		std::thread buildWorldThread;
		bool isBuildWorldThreadEnd = false;
	};
}
