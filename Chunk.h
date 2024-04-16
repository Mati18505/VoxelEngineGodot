#pragma once
#include "VoxelTypes.h"
#include <scene/resources/mesh.h>

namespace Voxel {
	struct Block;
	class World;
	class ChunkColumn;
	
	class Chunk {
	public:
		ChunkColumn& chunkColumn;
		const World& world;
	
		Chunk(Vector3 chunkWorldPosition, const World &worldParent, ChunkColumn &chunkColumnParent, int chunkHeightInColumn);
		
		void DrawChunk();
		void DeleteObject();

		const Block &GetBlockAt(const Vector3 &position) const;

		// Zwraca sąsiedni chunk lub nullptr jeśli nie istnieje.
		Chunk *GetNeighbour(BlockSide side);
		const Chunk *GetNeighbour(BlockSide side) const;
	
		~Chunk();

	private:
		RID chunkActor;
		Ref<Mesh> chunkMesh;
		Transform3D transform{};

		const int chunkHeight;

		void CreateActorIfEmpty();
		void UpdateMesh();
		Chunk *GetNeighbourImpl(BlockSide side) const;
	};
}
