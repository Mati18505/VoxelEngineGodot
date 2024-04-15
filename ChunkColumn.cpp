#include "ChunkColumn.h"
#include "World.h"
#include "Chunk.h"
#include "Block.h"
#include "BlockType.h"
#include <iostream>
#include <math.h>
#include "TerrainGenerator.h"
#include "Tools/Profiler.h"

#define chunkSize worldParent->config.chunkSize
#define chunkScaledSize worldParent->config.chunkScaledSize
#define worldScale worldParent->config.worldScale

#define Array3D(x, y, z, width, height) ((y) * width * height) + ((z) * width) + (x)
#define Chunk3DArray(x, y, z) Array3D(x, y, z, chunkSize, chunkSize * worldParent->config.columnHeight)

namespace Voxel {
	ChunkColumn::ChunkColumn(Vector3 chunkColumnWorldPos, World* worldParent, const int columnHeight) :
			chunkBlocks(Vector3i(chunkSize, chunkSize, chunkSize * columnHeight)) {
		chunks = new Chunk * [columnHeight];
		SM_PROFILE_ZONE;
		this->columnHeight = columnHeight;
		this->columnPosInWorld = chunkColumnWorldPos;
		this->worldParent = worldParent;
		GenerateChunks();
		GenerateStructures();
		this->status = ChunkStatus::GENERATED;
	}
	
	void ChunkColumn::GenerateChunks() {
		SM_PROFILE_ZONE;
		for (int chunkZ = 0; chunkZ < columnHeight; chunkZ++) 
			chunks[chunkZ] = new Chunk(columnPosInWorld + Vector3(0, 0, chunkZ * chunkScaledSize), worldParent, this, chunkZ * chunkSize);
	
		worldParent->terrainGenerator->GenerateTerrain(chunkBlocks, { (int)columnPosInWorld.x / worldScale, (int)columnPosInWorld.y / worldScale });
	}
	
	void ChunkColumn::GenerateStructures() {
		SM_PROFILE_ZONE;
		worldParent->terrainGenerator->GenerateStructures(chunkBlocks, { (int)columnPosInWorld.x / worldScale, (int)columnPosInWorld.y / worldScale });
	}
	
	void ChunkColumn::DrawChunks() {
		SM_PROFILE_ZONE;
		for (int i = 0; i < columnHeight; i++)
			chunks[i]->DrawChunk();
		this->status = ChunkStatus::DRAWN;
		toDraw = false;
	}
	
	void ChunkColumn::AddChunksObjects(Vector3 chunkColumnWorldPos) {
		toDraw = true;
	}
	
	
	void ChunkColumn::SetBlock(Vector3i position, BlockID blockID) {
		SM_PROFILE_ZONE;
		int chunkHeight = floor(position.z / chunkSize);
	
		this->chunkBlocks.At({ position.x, position.y, position.z }) = Block(blockID);
	
		int posZInChunk = position.z % chunkSize;
	
		Chunk* currentChunk = chunks[chunkHeight];
		
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
				if (chunk->chunkColumn->status == ChunkStatus::DRAWN)
					chunk->DrawChunk(); // TODO: wielowątkowość: ustawianie flagi needDraw
			}
		}
	}
	
	void ChunkColumn::ApplyModifiations()
	{
		while (!blocksModifications.empty())
		{
			VoxelMod mod = blocksModifications.front();
			blocksModifications.pop();
			this->chunkBlocks.At({ mod.pos.x, mod.pos.y, mod.pos.z }) = Block(mod.id);
		}
	}
	
	Block& ChunkColumn::GetBlockAt(Vector3i position) {
		return chunkBlocks.At({ position.x, position.y, position.z });
	}
	
	void ChunkColumn::DeleteChunksObjects() {
		SM_PROFILE_ZONE;
		toDraw = false;
		for (int i = 0; i < columnHeight; i++)
			chunks[i]->DeleteObject();
		this->status = ChunkStatus::GENERATED;
	}
	
	ChunkColumn* ChunkColumn::GetNeighbour(BlockSide side)
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
		if (neighbours[neighbourIndex] == nullptr)
		{
			ChunkPos neighbourPos = worldParent->GenerateChunkColumnID({ columnPosInWorld.x + chunkScaledSize * neighbourDirection.x , columnPosInWorld.y + chunkScaledSize * neighbourDirection.y, 0 });
			auto finded = worldParent->chunks.find(neighbourPos);
			if (!(finded == worldParent->chunks.end()))
			{
				neighbours[neighbourIndex] = finded->second;
			}
		}
		return neighbours[neighbourIndex];
	}
	
	ChunkColumn::~ChunkColumn()
	{
		for (int i = 0; i < columnHeight; i++)
			delete chunks[i];
		delete[] chunks;
	}
}
