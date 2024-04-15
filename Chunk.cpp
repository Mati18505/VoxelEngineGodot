#pragma once 
#include "Chunk.h"

#include "Tools/ActorManager.h"

#include "Block.h"
#include "World.h"
#include "ChunkColumn.h"
#include "VoxelMesher.h"
#include "BlockType.h"
#include "VoxelNode.h"
#include "MaterialDictionary.h"
#include "Tools/Profiler.h"

#define chunkSize world->config.chunkSize
#define chunkScaledSize world->config.chunkScaledSize
#define Chunk3DArray(x, y, z) Array3D(x, y, z, chunkSize, chunkSize * world->config.columnHeight)

#define ConvertYZ(vec3) Vector3(vec3.x, vec3.z, vec3.y)

namespace Voxel {
	Chunk::Chunk(Vector3 chunkWorldPosition, World* worldParent, ChunkColumn* chunkColumnParent, char chunkHeight)
	{
		world = worldParent;
		transform = Transform3D();
		transform.origin = ConvertYZ(chunkWorldPosition);
		Vector3 scale(world->config.worldScale, world->config.worldScale, world->config.worldScale);
		transform.scale(scale);
		chunkColumn = chunkColumnParent;
		this->chunkHeight = chunkHeight;
	}
	
	void Chunk::DrawChunk() {
		SM_PROFILE_ZONE;
		chunkMesh = world->gameMode.voxelMesher->CreateMesh(chunkColumn->chunkBlocks, chunkHeight, *this);
		CreateActorIfEmpty();
		UpdateMesh();
	}
	
	const Block& Chunk::GetBlockAt(const Vector3& position) const {
		int posZ = int(position.z) + chunkHeight;
	
		return chunkColumn->chunkBlocks.At({ int(position.x), int(position.y), int(posZ) });
	}
	
	const Chunk* Chunk::GetNeighbour(BlockSide side) const {
		return GetNeighbourImpl(side);
	}
	
	Chunk* Chunk::GetNeighbour(BlockSide side) {
		return GetNeighbourImpl(side);
	}

	Chunk* Chunk::GetNeighbourImpl(BlockSide side) const {
		int chunkHeightIndex = chunkHeight / chunkSize;
		int neighbourChunkIndex = chunkHeightIndex;
	
		if (side == BlockSide::TOP)
			neighbourChunkIndex++;
		else if (side == BlockSide::BOTTOM)
			neighbourChunkIndex--;
		else
		{
			ChunkColumn* neighbourColumn = chunkColumn->GetNeighbour(side);
			if (neighbourColumn == nullptr)
				return nullptr;
			return neighbourColumn->chunks[chunkHeightIndex];
		}
	
		if (neighbourChunkIndex < 0 || neighbourChunkIndex >= world->config.columnHeight)
			return nullptr;
	
		return chunkColumn->chunks[neighbourChunkIndex];
	}

	void Chunk::CreateActorIfEmpty() {
		SM_PROFILE_ZONE;
		if (chunkActor.is_null())
		{
			world->gameMode.actorManagerQueue.AddFunction([=]() {
				ActorManager::Get().CreateActor(chunkActor, transform);
			});
		}
	}

	void Chunk::UpdateMesh() {
		SM_PROFILE_ZONE;
		world->gameMode.actorManagerQueue.AddFunction([=]() {
			ActorManager::Get().UpdateMesh(chunkActor, chunkMesh);
		});
	}

	void Chunk::DeleteObject()
	{
		SM_PROFILE_ZONE;
		world->gameMode.actorManagerQueue.AddFunction([chunkActor = chunkActor]() {
			ActorManager::Get().DestroyActor(chunkActor);
		});
		chunkActor = RID();
	}
	
	Chunk::~Chunk()
	{
		DeleteObject();
	}
}
