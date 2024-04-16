#pragma once
#include "World.h"

#include <sstream>
#include "BlockType.h"
#include "Block.h"
#include "ChunkColumn.h"
#include "Biome.h"
#include "TerrainGenerator.h"
#include "Tools/Profiler.h"

namespace Voxel {
	World::World(GameMode &gameMode, std::vector<std::unique_ptr<Biome>> biomes) :
		biomes(std::move(biomes)), gameMode(gameMode), config(gameMode.config)
	{}
	
	void World::Start(Vector3 playerLocation) {
		SM_PROFILE_ZONE;
		print_line("start world\n");
			
		terrainGenerator = std::make_unique<TerrainGenerator>(this);
	
		Update(playerLocation);
		if(config.useThreading)
			buildWorldThread = std::thread(&World::BuildWorld, this);
	}
	
	void World::Update(Vector3 playerLocation) {
		SM_PROFILE_ZONE;
		UpdatePlayerPos(playerLocation);
	
		if (currentPlayerPos != lastPlayerPos)
		{
			lastPlayerPos = currentPlayerPos;
			GenerateWorld();
			if (!config.useThreading)
				BuildWorld();
		}
	}
	
	void World::BuildWorld() {
		SM_PROFILE_ZONE;
		if (config.useThreading) {
			SM_PROFILE_SET_THREAD_NAME("BuildWorld thread");
			while (!isBuildWorldThreadEnd) {
				for (auto& pair : chunks)
				{
					ChunkPos chunkPos = pair.first;
					auto chunk = pair.second;
	
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
				auto chunk = pair.second;
	
				if(chunk->toDraw == true)
					chunk->DrawChunks();
			}
		}
	}
		
	
	void World::GenerateWorld() {
		SM_PROFILE_ZONE;

		const int generateDistance = config.renderDistance + 2;
		const int generateStructsDistance = config.renderDistance + 1;
	
		int chunksGenerated = 0, chunksToDrawCount = 0, chunksToDelete = 0;
	
		{
			SM_PROFILE_ZONE_NAMED("Generate Pass");

			for (int y = -generateDistance + currentPlayerPos.y; y <= generateDistance + currentPlayerPos.y; y++) {
				for (int x = -generateDistance + currentPlayerPos.x; x <= generateDistance + currentPlayerPos.x; x++) {
					Vector3 chunkPos = Vector3(x * config.chunkScaledSize, y * config.chunkScaledSize, 0);
					ChunkPos chunkID = GenerateChunkColumnID(chunkPos);

					auto finded = chunks.find(chunkID);
					if (finded == chunks.end()) {
						auto chunkColumn = std::make_shared<ChunkColumn>(chunkPos, this, config.columnHeight);
						chunks.emplace(chunkID, std::move(chunkColumn));
						chunksGenerated++;
					}
				}
			}
		}
	
		{
			SM_PROFILE_ZONE_NAMED("Modifications Pass");

			std::vector<std::shared_ptr<ChunkColumn>> chunksToApplyModifiations;
			std::queue<VoxelMod> overdueBlocksModifications; // Przechowuje modyfikacje, które nie mogły być zastosowane, ponieważ ich chunk nie został jeszcze wygenerowany.
			while (!blocksModifications.empty()) {
				VoxelMod mod = blocksModifications.front();
				blocksModifications.pop();

				Vector3 blockScaledPos = (Vector3)mod.pos * config.worldScale;
				Vector3 chunkPos = GetChunkColumnPosByBlockWorldPosition(blockScaledPos);

				auto finded = chunks.find(GenerateChunkColumnID(chunkPos));
				if (!(finded == chunks.end())) {
					Vector3i blockPosInChunk = Vector3i(blockScaledPos - chunkPos) / config.worldScale;

					VoxelMod chunkMod{ mod.id, blockPosInChunk };

					finded->second->blocksModifications.push(chunkMod);
					chunksToApplyModifiations.push_back(finded->second);
				} else
					overdueBlocksModifications.push(mod); // TODO: zamiast wpisywać do tej listy, przechowywać metadane w chunkach nie wygenerowanych.
			}
			blocksModifications = overdueBlocksModifications;

			for (auto& chunk : chunksToApplyModifiations) {
				chunk->ApplyModifiations();
			}
		}
		
		std::vector<ChunkPos> chunksInRenderDistance;
	
		{
			SM_PROFILE_ZONE_NAMED("Render Pass");

			for (int y = -config.renderDistance + currentPlayerPos.y; y <= config.renderDistance + currentPlayerPos.y; y++) {
				for (int x = -config.renderDistance + currentPlayerPos.x; x <= config.renderDistance + currentPlayerPos.x; x++) {
					Vector3 chunkPos = Vector3(x * config.chunkScaledSize, y * config.chunkScaledSize, 0);
					ChunkPos chunkID = GenerateChunkColumnID(chunkPos);

					auto finded = chunks.find(chunkID);
					if (!(finded == chunks.end())) {
						chunksInRenderDistance.push_back(chunkID);

						if (finded->second->status == ChunkColumn::ChunkStatus::GENERATED && finded->second->toDraw == false) {
							finded->second->AddChunksObjects(chunkPos);
							chunksToDrawCount++;
						}
					}
				}
			}
		}
		
	
		{
			SM_PROFILE_ZONE_NAMED("Delete Pass");
			for (auto &pair : chunks) {
				ChunkPos chunkPos = pair.first;
				auto& chunk = pair.second;

				if (chunk->status == ChunkColumn::ChunkStatus::DRAWN || chunk->toDraw == true) {
					auto finded = std::find(chunksInRenderDistance.begin(), chunksInRenderDistance.end(), chunkPos);
					if (finded == chunksInRenderDistance.end()) {
						chunk->DeleteChunksObjects();
						chunksToDelete++;
					}
				}
			}
		}
		
		std::stringstream ss;
		ss << "chunks generated: " << chunksGenerated << ", chunks to draw: " << chunksToDrawCount << ", chunks to delete: " << chunksToDelete;
		print_line(ss.str().c_str());
		SM_PROFILE_LOG(ss.str().c_str(), ss.str().size());
	}
	
	ChunkPos World::GenerateChunkColumnID(Vector3 chunkPos) {
		return ChunkPos(int(std::floor(chunkPos.x)), int(std::floor(chunkPos.y)));
	}
	
	Vector3 World::GetChunkColumnPosByBlockWorldPosition(Vector3 blockWorldPosition)
	{
		return Vector3(floor(blockWorldPosition.x / config.chunkScaledSize) * config.chunkScaledSize, floor(blockWorldPosition.y / config.chunkScaledSize) * config.chunkScaledSize, 0);
	}
	
	const BlockType* World::GetBlockTypeInWorld(Vector3 blockWorldPosition)
	{
		SM_PROFILE_ZONE;
		Vector3 chunkPos = GetChunkColumnPosByBlockWorldPosition(blockWorldPosition);
		Vector3 blockInChunkPos = blockWorldPosition - chunkPos;
		blockInChunkPos /= config.worldScale;
	
		if (blockInChunkPos.z >= config.columnHeight * config.chunkSize || blockInChunkPos.z < 0)
			return nullptr;
	
		ChunkPos chunkID = GenerateChunkColumnID(chunkPos);
	
		auto finded = chunks.find(chunkID); // Szukanie chunka w map chunks.
		if (!(finded == chunks.end())){
			int blockID = finded->second->GetBlockAt(Vector3i(blockInChunkPos)).typeID;
			return &gameMode.blockTypes[blockID];
		}	
		return nullptr;
	}
	
	static Vector3 RoundVectorInScale(Vector3 v, const int scale) {
		return { round(v.x / scale) * scale, round(v.y / scale) * scale, round(v.z / scale) * scale };
	}
	
	bool World::BlockRayCast(Vector3 startPoint, Vector3 direction, Vector3* hitPoint, float range, Vector3* lastHitPoint)
	{
		SM_PROFILE_ZONE;
		range *= config.worldScale;
	
	    Vector3 currentPos = startPoint;
	    float currentDistance = 0;
	    bool isExceededRange = false;


	    bool isBlockTypeSolid = IsBlockSolid(RoundVectorInScale(currentPos, config.worldScale));
	
	    while (!isExceededRange && !isBlockTypeSolid)
	    {
			currentPos += direction * config.blockRayCastIncrement;
			currentDistance += config.blockRayCastIncrement;
	
	        if (range < currentDistance)
	            isExceededRange = true;
	
			isBlockTypeSolid = IsBlockSolid(RoundVectorInScale(currentPos, config.worldScale));
	    }
	
	    *hitPoint = RoundVectorInScale(currentPos, config.worldScale);
	
		if (lastHitPoint != nullptr)
			*lastHitPoint = RoundVectorInScale(currentPos - direction * config.blockRayCastIncrement, config.worldScale);
	
	    if (!isExceededRange)
	        return true;
	    return false;
	}
	
	
	void World::SetBlock(Vector3 blockPositionInWorld, BlockID blockID) {
		SM_PROFILE_ZONE;
		Vector3 chunkPos = GetChunkColumnPosByBlockWorldPosition(blockPositionInWorld);
		auto finded = chunks.find(GenerateChunkColumnID(chunkPos)); // Szukanie chunka w map chunks.
		if (!(finded == chunks.end())) {
			Vector3i blockPosInChunk = Vector3i(blockPositionInWorld - chunkPos);
			blockPosInChunk /= config.worldScale;
			finded->second->SetBlock(blockPosInChunk, blockID);
		}
	}
	
	World::~World()
	{
		if (config.useThreading) {
			isBuildWorldThreadEnd = true;
			buildWorldThread.join();
		}
		print_line("World deleted\n");
	}
	
	void World::UpdatePlayerPos(Vector3 PlayerLocation) {
		
		currentPlayerPos.x = floor(PlayerLocation.x / config.chunkScaledSize);
		currentPlayerPos.y = floor(PlayerLocation.y / config.chunkScaledSize);
	}

	bool World::IsBlockSolid(Vector3 pos) {
		if (const BlockType *blockType = GetBlockTypeInWorld(pos))
			return blockType->isSolid;

		return false;
	}

}
