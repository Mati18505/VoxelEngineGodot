#include "ChunkColumn.h"
#include "World.h"
#include "Chunk.h"
#include "Block.h"
#include "BlockType.h"
#include <iostream>
#include <math.h>
#include "TerrainGenerator.h"
#include "Tools/Profiler.h"

#define chunkSize worldParent.config.chunkSize
#define chunkScaledSize worldParent.config.chunkScaledSize
#define worldScale worldParent.config.worldScale

#define Array3D(x, y, z, width, height) ((y) * width * height) + ((z) * width) + (x)
#define Chunk3DArray(x, y, z) Array3D(x, y, z, chunkSize, chunkSize * worldParent.config.columnHeight)

namespace Voxel {
	ChunkColumn::ChunkColumn(Vector3 chunkColumnWorldPos, const World& worldParent, const int columnHeight) :
			chunkBlocks(Vector3i(chunkSize, chunkSize, chunkSize * columnHeight)),
			columnHeight(columnHeight), worldParent(worldParent), columnPosInWorld(std::move(chunkColumnWorldPos))
	{
		SM_PROFILE_ZONE;
		chunks.reserve(columnHeight);
		GenerateChunks();
		GenerateStructures();
	}
	
	void ChunkColumn::GenerateChunks() {
		SM_PROFILE_ZONE;
		for (int chunkZ = 0; chunkZ < columnHeight; chunkZ++) 
			chunks.push_back(std::make_unique<Chunk>(columnPosInWorld + Vector3(0, 0, chunkZ * chunkScaledSize), worldParent, *this, chunkZ * chunkSize));
	
		worldParent.terrainGenerator->GenerateTerrain(chunkBlocks, { (int)columnPosInWorld.x / worldScale, (int)columnPosInWorld.y / worldScale });
	}
	
	void ChunkColumn::GenerateStructures() {
		SM_PROFILE_ZONE;
		worldParent.terrainGenerator->GenerateStructures(chunkBlocks, { (int)columnPosInWorld.x / worldScale, (int)columnPosInWorld.y / worldScale });
	}
	
	void ChunkColumn::DrawChunks() {
		SM_PROFILE_ZONE;
		for (int i = 0; i < columnHeight; i++)
			chunks[i]->DrawChunk();
		status = ChunkStatus::DRAWN;
		toDraw = false;
	}
	
	void ChunkColumn::AddChunksObjects() {
		toDraw = true;
	}
	
	void ChunkColumn::SetBlock(const Vector3i &position, BlockID blockID) {
		SM_PROFILE_ZONE;
		int chunkHeight = floor(position.z / chunkSize);
	
		chunkBlocks.At({ position.x, position.y, position.z }) = Block(blockID);
	
		int posZInChunk = position.z % chunkSize;
	
		Chunk* currentChunk = chunks[chunkHeight].get();
		
		std::vector<Chunk*> chunksToRedraw;
		
		chunksToRedraw.push_back(currentChunk);
		
		if (position.x == 0)
			chunksToRedraw.push_back(currentChunk->GetNeighbour(LEFT));
		if (position.x >= chunkSize-1)
			chunksToRedraw.push_back(currentChunk->GetNeighbour(RIGHT));
		if (position.y == 0)
			chunksToRedraw.push_back(currentChunk->GetNeighbour(BACK));
		if (position.y >= chunkSize-1)
			chunksToRedraw.push_back(currentChunk->GetNeighbour(FRONT));
		if (posZInChunk == 0)
			chunksToRedraw.push_back(currentChunk->GetNeighbour(BOTTOM));
		if (posZInChunk >= chunkSize-1)
			chunksToRedraw.push_back(currentChunk->GetNeighbour(TOP));
	
		for (Chunk* chunk : chunksToRedraw)
		{
			if (chunk != nullptr)
			{
				if (chunk->chunkColumn.status == ChunkStatus::DRAWN)
					chunk->DrawChunk(); // TODO: wielowątkowość: ustawianie flagi needDraw
			}
		}
	}
	
	void ChunkColumn::ApplyModifications()
	{
		while (!blocksModifications.empty())
		{
			const VoxelMod& mod = blocksModifications.front();
			chunkBlocks.At({ mod.pos.x, mod.pos.y, mod.pos.z }) = Block(mod.id);
			blocksModifications.pop();
		}
	}
	
	const Block &ChunkColumn::GetBlockAt(const Vector3i &position) const {
		return chunkBlocks.At(position);
	}
	
	void ChunkColumn::AddBlockModification(VoxelMod mod) {
		blocksModifications.push(std::move(mod));
	}

	void ChunkColumn::DeleteChunksObjects() {
		SM_PROFILE_ZONE;
		toDraw = false;
		for (int i = 0; i < columnHeight; i++)
			chunks[i]->DeleteObject();
		status = ChunkStatus::GENERATED;
	}
	
	std::weak_ptr<ChunkColumn> ChunkColumn::GetNeighbour(BlockSide side) const
	{
		Vector3i neighbourDirection;
		int neighbourIndex;
		switch (side)
		{
		case LEFT:
			neighbourIndex = 0;
			neighbourDirection = { -1,0,0 };
			break;
		case RIGHT:
			neighbourIndex = 1;
			neighbourDirection = { 1,0,0 };
			break;
		case FRONT:
			neighbourIndex = 2;
			neighbourDirection = { 0,1,0 };
			break;
		case BACK:
			neighbourIndex = 3;
			neighbourDirection = { 0,-1,0 };
			break;
		default:
			throw std::invalid_argument("Invalid side!");
			break;
		}
	
		// Jeśli chunka nie ma w tablicy neighbours to go szukamy w świecie.
		if (neighbours[neighbourIndex].expired())
		{
			ChunkPos neighbourPos = worldParent.GenerateChunkColumnID({ columnPosInWorld.x + chunkScaledSize * neighbourDirection.x , columnPosInWorld.y + chunkScaledSize * neighbourDirection.y, 0 });
			auto finded = worldParent.chunks.find(neighbourPos);
			if (finded != worldParent.chunks.end())
			{
				neighbours[neighbourIndex] = finded->second;
			}
		}
		return neighbours[neighbourIndex];
	}
}
