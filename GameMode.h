#pragma once
#include "VoxelTypes.h"
#include "TextureDictionary.h"
#include "MaterialDictionary.h"
#include "BlockTypeStorage.h"
#include "Tools/ActorManagerQueue.h"
#include "vendor/nlohmann/json.hpp"
#include "Tools/ThreadPool/ThreadPool.h"

class VoxelNode;
namespace Voxel {
	class VoxelMesher;
	class World;
	struct Biome;

	class GameMode {
	public:
		struct Config {
			int columnHeight = 8;
			int renderDistance = 10;
			int seed = 0;
			bool useThreading = false;
			float blockRayCastIncrement = 0.5f;

			int worldScale = 1;
			int chunkSize = 16;
			int chunkScaledSize = worldScale * chunkSize;
		} const config;

		GameMode(const VoxelNode &voxelNode, Config config, Vector3 playerPos, const Dictionary &materials, const Dictionary &textures, const nlohmann::json &biomesJson, const nlohmann::json &blocksJson);
		~GameMode();
		void Update(Vector3 playerLocation);

		std::unique_ptr<World> world;
		ActorManagerQueue actorManagerQueue;
		const std::unique_ptr<const VoxelMesher> voxelMesher = std::make_unique<VoxelMesher>(*this);
		const TextureDictionary textureDictionary;
		const MaterialDictionary materialDictionary;
		const BlockTypeStorage blockTypes;
		tk::ThreadPool threadPool{ 12 };

	private:
		const VoxelNode &voxelNode;

		BlockTypeStorage GenerateBlockTypes(const nlohmann::json &jsonFile) const;
		std::vector<std::unique_ptr<const Biome>> GenerateBiomes(const nlohmann::json &biomesJson) const;
	};
}
