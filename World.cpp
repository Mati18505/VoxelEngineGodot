#pragma once
#include "World.h"

#include <sstream>
#include "BlockType.h"
#include "Block.h"
#include "ChunkColumn.h"
#include "Biome.h"
#include "TerrainGenerator.h"
#include "Tools/Profiler.h"
#include "VoxelMesher.h"

namespace Voxel {
	World::World(GameMode &gameMode, std::vector<std::unique_ptr<const Biome>> biomes) :
		biomes(std::move(biomes)), gameMode(gameMode), config(gameMode.config)
	{}
	
	void World::Start(const Vector3 &playerLocation) {
		SM_PROFILE_ZONE;
		print_line("start world\n");
			
		terrainGenerator = std::make_unique<TerrainGenerator>(*this);
	
		Update(playerLocation);
	}
	
	void World::Update(const Vector3 &playerLocation) {
		SM_PROFILE_ZONE;
		UpdatePlayerPos(playerLocation);
	
		if (currentPlayerPos != lastPlayerPos)
		{
			lastPlayerPos = currentPlayerPos;
			GenerateWorld();
		}
		BuildWorld();
	}
	
	void World::BuildWorld() {
		SM_PROFILE_ZONE;
		for (auto &[chunkPos, chunkColumn] : chunks)
		{
			if(chunkColumn->GetToDraw() == true)
			{
				for (int i = 0; i < config.columnHeight; i++)
				{
					Voxel::Array3d<Block> voxelData = chunkColumn->GetBlockStorage();

					chunkMeshingJobs.push_back(std::async([=, voxelData = std::move(voxelData), &voxelMesher = gameMode.voxelMesher, &config = config]() -> ChunkMeshingJobResult {
						SM_PROFILE_ZONE_NAMED("Chunk Meshing Job");
						ChunkMeshingJobResult result;

						result.chunkPos = chunkPos;
						result.chunkMesh = voxelMesher->CreateMesh({ std::move(voxelData), uint8_t(i * config.chunkSize) });
						result.chunkInColumn = i;
						return result;
					}));
					chunkColumn->SetToDraw(false);
					chunkColumn->SetStatus(ChunkColumn::ChunkStatus::DRAWN);
				}
			}
		}

		for (auto it = chunkMeshingJobs.begin(); it < chunkMeshingJobs.end(); it++) {
			auto &future = *it;
			using namespace std::chrono_literals;

			if (future.wait_for(0ms) == std::future_status::ready) {
				auto result = future.get();

				if (auto pair = chunks.find(result.chunkPos); pair != chunks.end()) {
					auto &chunkColumn = pair->second;
					auto &chunk = chunkColumn->GetChunk(result.chunkInColumn);
					chunk.SetMesh(std::move(result.chunkMesh));
				}
				it = chunkMeshingJobs.erase(it);
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
						auto chunkColumn = std::make_shared<ChunkColumn>(chunkPos, *this, config.columnHeight);
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

					finded->second->AddBlockModification(chunkMod);
					chunksToApplyModifiations.push_back(finded->second);
				} else
					overdueBlocksModifications.push(mod); // TODO: zamiast wpisywać do tej listy, przechowywać metadane w chunkach nie wygenerowanych.
			}
			blocksModifications = overdueBlocksModifications;

			for (auto& chunk : chunksToApplyModifiations) {
				chunk->ApplyModifications();
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

						if (finded->second->GetStatus() == ChunkColumn::ChunkStatus::GENERATED && finded->second->GetToDraw() == false) {
							finded->second->AddChunksObjects();
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

				if (chunk->GetStatus() == ChunkColumn::ChunkStatus::DRAWN || chunk->GetToDraw() == true) {
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
	
	ChunkPos World::GenerateChunkColumnID(const Vector3 &chunkPos) const {
		return ChunkPos(int(std::floor(chunkPos.x)), int(std::floor(chunkPos.y)));
	}
	
	Vector3 World::GetChunkColumnPosByBlockWorldPosition(const Vector3 &blockWorldPosition) const
	{
		return Vector3(floor(blockWorldPosition.x / config.chunkScaledSize) * config.chunkScaledSize, floor(blockWorldPosition.y / config.chunkScaledSize) * config.chunkScaledSize, 0);
	}
	
	const BlockType *World::GetBlockTypeInWorld(const Vector3 &blockWorldPosition) const
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
	
	World::RayCastResult World::BlockRayCast(Vector3 startPoint, const Vector3 &direction, float range) const
	{
		SM_PROFILE_ZONE;
		range *= config.worldScale;
	
	    Vector3 currentPos = std::move(startPoint);
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

		RayCastResult result;
	
	    result.hitPoint = RoundVectorInScale(currentPos, config.worldScale);
		result.previousHitPoint = RoundVectorInScale(currentPos - direction * config.blockRayCastIncrement, config.worldScale);
	    result.hitted = !isExceededRange;
		return result;
	}
	
	
	void World::SetBlock(const Vector3 &blockPositionInWorld, BlockID blockID) {
		SM_PROFILE_ZONE;
		Vector3 chunkPos = GetChunkColumnPosByBlockWorldPosition(blockPositionInWorld);
		auto finded = chunks.find(GenerateChunkColumnID(chunkPos)); // Szukanie chunka w map chunks.
		if (!(finded == chunks.end())) {
			Vector3i blockPosInChunk = Vector3i(blockPositionInWorld - chunkPos);
			blockPosInChunk /= config.worldScale;
			finded->second->SetBlock(blockPosInChunk, blockID);
		}
	}
	
	void World::UpdatePlayerPos(const Vector3 &PlayerLocation) {
		
		currentPlayerPos.x = floor(PlayerLocation.x / config.chunkScaledSize);
		currentPlayerPos.y = floor(PlayerLocation.y / config.chunkScaledSize);
	}

	bool World::IsBlockSolid(const Vector3 &pos) const {
		if (const BlockType *blockType = GetBlockTypeInWorld(pos))
			return blockType->isSolid;

		return false;
	}

}
