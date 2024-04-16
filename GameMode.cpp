#include "GameMode.h"
#include "Tools/Profiler.h"
#include "VoxelNode.h"
#include "Tools/ActorManager.h"
#include "Biome.h"
#include "World.h"
#include "VoxelMesher.h"

namespace Voxel {


	GameMode::GameMode(const VoxelNode &voxelNode, Config config, Vector3 playerPos, const Dictionary &materials, const Dictionary &textures, const nlohmann::json &biomesJson, const nlohmann::json &blocksJson) :
		voxelNode(voxelNode), config(std::move(config)), materialDictionary(materials), textureDictionary(textures), blockTypes(GenerateBlockTypes(blocksJson))
	{
		actorManagerQueue.Clear();

		world = std::make_unique<World>(*this, GenerateBiomes(biomesJson));

		world->Start(playerPos);
	}


	GameMode::~GameMode() {
		actorManagerQueue.Resolve();
	}

	void GameMode::Update(Vector3 playerLocation) {
		world->Update(playerLocation);
		actorManagerQueue.Resolve();
	}

	BlockTypeStorage GameMode::GenerateBlockTypes(const nlohmann::json &jsonFile) const {
		SM_PROFILE_ZONE;
		using namespace Voxel;

		std::vector<BlockType> blockTypes;

		for (int i = 0; i < jsonFile["blocks"].size(); i++) {
			Json blockP = jsonFile["blocks"][i];
			std::string materialName = blockP.contains("material") ? blockP["material"] : "default";
			bool isTransparent = blockP["isTransparent"] == "true";
			bool isTranslucent = blockP.contains("translucent") ? (bool)blockP["translucent"] : isTransparent;

			BlockType &block = blockTypes.emplace_back(blockP["name"], isTransparent, blockP["isEverySideSame"] == "true", materialName, isTranslucent, blockP["name"] != "air");

			if (blockP["isEverySideSame"] == "true")
				block.sideTextureIndex = textureDictionary.GetBlockTextureIndex(blockP["textures"]["side"]);
			else {
				block.sideTextureIndex = textureDictionary.GetBlockTextureIndex(blockP["textures"]["side"]);
				block.bottomTextureIndex = textureDictionary.GetBlockTextureIndex(blockP["textures"]["bottom"]);
				block.topTextureIndex = textureDictionary.GetBlockTextureIndex(blockP["textures"]["top"]);
			}
		}
		return BlockTypeStorage{ blockTypes };
	}

	std::vector<std::unique_ptr<const Biome>> GameMode::GenerateBiomes(const nlohmann::json &biomesJson) const {
		SM_PROFILE_ZONE;
		std::vector<std::unique_ptr<const Biome>> biomes;

		for (nlohmann::json biomeJ : biomesJson["biomes"]) {
			Biome biome;
			biome.biomeName = biomeJ["name"];
			biome.atmosphereBlock = blockTypes.GetBlockTypeIDFromName(biomeJ["atmosphere"]);
			biome.layer1stBlock = blockTypes.GetBlockTypeIDFromName(biomeJ["layer1st"]);
			biome.layer2ndBlock = blockTypes.GetBlockTypeIDFromName(biomeJ["layer2nd"]);
			biome.layer3rdBlock = blockTypes.GetBlockTypeIDFromName(biomeJ["layer3rd"]);
			biome.majorFloraZoneScale = biomeJ["majorFloraZoneScale"];
			biome.majorFloraZoneThreshold = biomeJ["majorFloraZoneThreshold"];
			biome.majorFloraPlacementScale = biomeJ["majorFloraPlacementScale"];
			biome.majorFloraPlacementThreshold = biomeJ["majorFloraPlacementThreshold"];
			biome.placeMajorFlora = biomeJ["placeMajorFlora"];

			biomes.push_back(std::make_unique<const Biome>(biome));
		}

		return biomes;
	}
}
