#pragma once
#include "World.h"

#include <sstream>
#include "BlockType.h"
#include "Block.h"
#include "ChunkColumn.h"
#include "Biome.h"
#include "TerrainGenerator.h"
#include "VoxelNode.h"

namespace Voxel {
	World::World(VoxelNode* gameMode)
	{
		for (int i = 0; i < gameMode->materials.size(); i++)
		{
			String materialName = gameMode->materials.get_key_at_index(i);
			Ref<Material> material = gameMode->materials.get_value_at_index(i);
			blockMaterials.insert({ std::string(materialName.utf8()), material });
		}
	
		this->currentPlayerPos = Vector2i(0, 0);
		this->lastPlayerPos = Vector2i(-1, 0);
		this->gameMode = gameMode;
		isBuildWorldThreadEnd = false;
	}
	
	void World::Start(Json texturesJson, Json biomesJson, Vector3 playerLocation) {
		print_line("start world\n");
		print_line("\nTextures: \n");
		GenerateTextureIndexDictionary();
	
		for (auto& it : textureIndexDictionary)
		{
			print_line(std::string(it.first + " - " + std::to_string(it.second) + "\n").c_str());
		}
			
		terrainGenerator = std::make_unique<TerrainGenerator>(this);
	
		GenerateBlockTypes(texturesJson);
		GenerateBiomeTypes(biomesJson);
	
		Update(playerLocation);
		if(useThreading)
			buildWorldThread = std::thread(&World::BuildWorld, this);
	}
	
	void World::Update(Vector3 playerLocation) {
		UpdatePlayerPos(playerLocation);
	
		if (currentPlayerPos != lastPlayerPos)
		{
			lastPlayerPos = currentPlayerPos;
			GenerateWorld();
			if(!useThreading)
				BuildWorld();
		}
	}
	
	void World::BuildWorld() {
		if (useThreading) {
			while (!isBuildWorldThreadEnd) {
				for (auto& pair : chunks)
				{
					ChunkPos chunkPos = pair.first;
					ChunkColumn *chunk = pair.second;
	
					if (chunk->toDraw == true)
						chunk->DrawChunks();
				}
	
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}		
		}
		else {
			for (auto& pair : chunks)
			{
				ChunkPos chunkPos = pair.first;
				ChunkColumn *chunk = pair.second;
	
				if(chunk->toDraw == true)
					chunk->DrawChunks();
			}
		}
	}
		
	
	void World::GenerateWorld() {
		const int generateDistance = renderDistance + 2;
		const int generateStructsDistance = renderDistance + 1;
	
		int chunksGenerated = 0, chunksToDrawCount = 0, chunksToDelete = 0;
	
		// GeneratePass
		for (int y = -generateDistance + currentPlayerPos.y; y <= generateDistance + currentPlayerPos.y; y++)
		{
			for (int x = -generateDistance + currentPlayerPos.x; x <= generateDistance + currentPlayerPos.x; x++)
			{
				Vector3 chunkPos = Vector3(x * chunkScaledSize, y * chunkScaledSize, 0);
				ChunkPos chunkID = GenerateChunkColumnID(chunkPos);
	
				auto finded = chunks.find(chunkID);
				if (finded == chunks.end())
				{
					ChunkColumn* chunkColumn = new ChunkColumn(chunkPos, this, columnHeight);
					chunks.insert({ chunkID, chunkColumn });
					chunksGenerated++;
				}
			}
		}
	
		// ModificationsPass
		std::vector<ChunkColumn*> chunksToApplyModifiations;
		std::queue<VoxelMod> overdueBlocksModifications; // Przechowuje modyfikacje, które nie mogły być zastosowane, ponieważ ich chunk nie został jeszcze wygenerowany.
		while (!blocksModifications.empty())
		{
			VoxelMod mod = blocksModifications.front();
			blocksModifications.pop();
	
			Vector3 blockScaledPos = (Vector3)mod.pos * worldScale;
			Vector3 chunkPos = GetChunkColumnPosByBlockWorldPosition(blockScaledPos);
	
			auto finded = chunks.find(GenerateChunkColumnID(chunkPos));
			if (!(finded == chunks.end())) {
				Vector3i blockPosInChunk = Vector3i(blockScaledPos - chunkPos) / worldScale;
	
				VoxelMod chunkMod{ mod.id, blockPosInChunk };
	
				finded->second->blocksModifications.push(chunkMod);
				chunksToApplyModifiations.push_back(finded->second);
			}
			else
				overdueBlocksModifications.push(mod); // TODO: zamiast wpisywać do tej listy, przechowywać metadane w chunkach nie wygenerowanych.
		}
		blocksModifications = overdueBlocksModifications;
	
		for (ChunkColumn* chunk : chunksToApplyModifiations)
		{
			chunk->ApplyModifiations();
		}
	
		// RenderPass
		std::vector<ChunkPos> chunksInRenderDistance;
	
		for (int y = -renderDistance + currentPlayerPos.y; y <= renderDistance + currentPlayerPos.y; y++)
		{
			for (int x = -renderDistance + currentPlayerPos.x; x <= renderDistance + currentPlayerPos.x; x++)
			{
				Vector3 chunkPos = Vector3(x * chunkScaledSize, y * chunkScaledSize, 0);
				ChunkPos chunkID = GenerateChunkColumnID(chunkPos);
	
				auto finded = chunks.find(chunkID);
				if (!(finded == chunks.end())) {
					chunksInRenderDistance.push_back(chunkID);
	
					if (finded->second->status == ChunkColumn::ChunkStatus::GENERATED && finded->second->toDraw == false)
					{
						finded->second->AddChunksObjects(chunkPos);
						chunksToDrawCount++;
					}
				}
			}
		}
	
		// DeletePass
		for (auto& pair : chunks) {
			ChunkPos chunkPos = pair.first;
			ChunkColumn *chunk = pair.second;
	
			if (chunk->status == ChunkColumn::ChunkStatus::DRAWN || chunk->toDraw == true)
			{
				auto finded = std::find(chunksInRenderDistance.begin(), chunksInRenderDistance.end(), chunkPos);
				if (finded == chunksInRenderDistance.end())
				{
					chunk->DeleteChunksObjects();
					chunksToDelete++;
				}
			}
		}
		
		std::stringstream ss;
		ss << "chunks generated: " << chunksGenerated << ", chunks to draw: " << chunksToDrawCount << ", chunks to delete: " << chunksToDelete;
		print_line(ss.str().c_str());
	}
	
	/// <summary>
	/// Dodanie bloków na podstawie jsonFile
	/// </summary>
	/// <param name="jsonFile">Objekt Json na podstawie którego wygenerować typy bloków</param>
	void World::GenerateBlockTypes(Json jsonFile) {
		
		for (int i = 0; i < jsonFile["blocks"].size(); i++)
		{
			Json blockP = jsonFile["blocks"][i];
			std::string materialName = blockP.contains("material") ? blockP["material"] : "default";
			bool isTransparent = blockP["isTransparent"] == "true";
			bool isTranslucent = blockP.contains("translucent") ? (bool)blockP["translucent"] : isTransparent;
	
			BlockType* block = new BlockType(blockP["name"], isTransparent, blockP["isEverySideSame"] == "true", materialName, isTranslucent);
			if (blockP["isEverySideSame"] == "true") 
				block->SetBlockSideTextureIndex(GetBlockTextureIndex(blockP["textures"]["side"]));
			else {
				block->SetBlockSideTextureIndex(GetBlockTextureIndex(blockP["textures"]["side"]));
				block->SetBottomTextureIndex(GetBlockTextureIndex(blockP["textures"]["bottom"]));
				block->SetTopTextureIndex(GetBlockTextureIndex(blockP["textures"]["top"]));
			}
			blockTypes.push_back(block);
		}
	}
	
	void World::GenerateBiomeTypes(Json jsonFile)
	{
		for (Json biomeJ : jsonFile["biomes"])
		{
			biomes.push_back(std::make_unique<Biome>(biomeJ["name"]));
			Biome& biome = *biomes.back();
			biome.atmosphereBlock = GetBlockTypeIDFromName(biomeJ["atmosphere"]);
			biome.layer1stBlock = GetBlockTypeIDFromName(biomeJ["layer1st"]);
			biome.layer2ndBlock = GetBlockTypeIDFromName(biomeJ["layer2nd"]);
			biome.layer3rdBlock = GetBlockTypeIDFromName(biomeJ["layer3rd"]);
			biome.majorFloraZoneScale = biomeJ["majorFloraZoneScale"];
			biome.majorFloraZoneThreshold = biomeJ["majorFloraZoneThreshold"];
			biome.majorFloraPlacementScale = biomeJ["majorFloraPlacementScale"];
			biome.majorFloraPlacementThreshold = biomeJ["majorFloraPlacementThreshold"];
			biome.placeMajorFlora = biomeJ["placeMajorFlora"];
			
		}
	}
	
	BlockID World::GetBlockTypeIDFromName(std::string typeName)
	{
		auto finded = std::find_if(blockTypes.begin(), blockTypes.end(), [&typeName](BlockType* blockType) { return blockType->name == typeName; });
	
		if (finded == blockTypes.end())
			throw std::invalid_argument("BlockType name does not exist!");
	
		return finded - blockTypes.begin();
	}
	
	Ref<Material> World::GetBlockTypeMaterial(BlockType *blockType) {
		return blockMaterials.at(blockType->GetMaterialName());
	}
	
	Ref<Material> World::GetMaterialFromName(std::string name) {
		return blockMaterials.at(name);
	}
	
	int World::GetBlockTextureIndex(const std::string& blockName) {
		if (blockName == "" || textureIndexDictionary.count(blockName) == 0)
			return 0;
		else
			return textureIndexDictionary.at(blockName);
	}
	
	ChunkPos World::GenerateChunkColumnID(Vector3 chunkPos) {
		return ChunkPos(int(std::floor(chunkPos.x)), int(std::floor(chunkPos.y)));
	}
	
	Vector3 World::GetChunkColumnPosByBlockWorldPosition(Vector3 blockWorldPosition)
	{
		return Vector3(floor(blockWorldPosition.x / chunkScaledSize) * chunkScaledSize, floor(blockWorldPosition.y / chunkScaledSize) * chunkScaledSize, 0);
	}
	
	BlockType* World::GetBlockTypeInWorld(Vector3 blockWorldPosition)
	{
		Vector3 chunkPos = GetChunkColumnPosByBlockWorldPosition(blockWorldPosition);
		Vector3 blockInChunkPos = blockWorldPosition - chunkPos;
		blockInChunkPos /= worldScale;
	
		if (blockInChunkPos.z >= columnHeight * chunkSize || blockInChunkPos.z < 0)
			return nullptr;
	
		ChunkPos chunkID = GenerateChunkColumnID(chunkPos);
	
		auto finded = chunks.find(chunkID); // Szukanie chunka w map chunks.
		if (!(finded == chunks.end())){
			int blockID = finded->second->GetBlockAt(Vector3i(blockInChunkPos)).GetBlockTypeID();
			return blockTypes[blockID];
		}	
		return nullptr;
	}
	
	static Vector3 RoundVectorInScale(Vector3 v, const int scale) {
		return { round(v.x / scale) * scale, round(v.y / scale) * scale, round(v.z / scale) * scale };
	}
	
	bool World::BlockRayCast(Vector3 startPoint, Vector3 direction, Vector3* hitPoint, float range, Vector3* lastHitPoint)
	{
		range *= worldScale;
	
	    Vector3 currentPos = startPoint;
	    float currentDistance = 0;
	    bool isExceededRange = false;
	    bool isBlockTypeSolid;
	
		BlockType* blockType = GetBlockTypeInWorld(RoundVectorInScale(currentPos, worldScale));
		isBlockTypeSolid = blockType == nullptr ? false : blockType->GetIsSolid();
	
	    while (!isExceededRange && !isBlockTypeSolid)
	    {
	        currentPos += direction * blockRayCastIncrement;
	        currentDistance += blockRayCastIncrement;
	
	        if (range < currentDistance)
	            isExceededRange = true;
	
			blockType = GetBlockTypeInWorld(RoundVectorInScale(currentPos, worldScale));
			isBlockTypeSolid = blockType == nullptr ? false : blockType->GetIsSolid();
	    }
	
	    *hitPoint = RoundVectorInScale(currentPos, worldScale);
	
		if (lastHitPoint != nullptr)
			*lastHitPoint = RoundVectorInScale(currentPos - direction * blockRayCastIncrement, worldScale);
	
	    if (!isExceededRange)
	        return true;
	    return false;
	}
	
	
	void World::SetBlock(Vector3 blockPositionInWorld, BlockID blockID) {
		Vector3 chunkPos = GetChunkColumnPosByBlockWorldPosition(blockPositionInWorld);
		auto finded = chunks.find(GenerateChunkColumnID(chunkPos)); // Szukanie chunka w map chunks.
		if (!(finded == chunks.end())) {
			Vector3i blockPosInChunk = Vector3i(blockPositionInWorld - chunkPos);
			blockPosInChunk /= worldScale;
			finded->second->SetBlock(blockPosInChunk, blockID);
		}
	}
	
	World::~World()
	{
		if (useThreading) {
			isBuildWorldThreadEnd = true;
			buildWorldThread.join();
		}
	
		for (auto& it : chunks)
			delete it.second;
	
		for (auto& blockType : blockTypes)
			delete blockType;
		print_line("World deleted\n");
	}
	
	void World::UpdatePlayerPos(Vector3 PlayerLocation) {
		
		currentPlayerPos.x = floor(PlayerLocation.x / chunkScaledSize);
		currentPlayerPos.y = floor(PlayerLocation.y / chunkScaledSize);
	}
	
	void World::GenerateTextureIndexDictionary()
	{
		for (int textureIndex = 0; textureIndex < gameMode->textureArray->get_layers(); textureIndex++)
		{
			Ref<Texture2D> currentTexture = gameMode->textureArray->get_layer_data(textureIndex);
			textureIndexDictionary[std::string(currentTexture->get_name().utf8())] = textureIndex;
		}
	}
}
