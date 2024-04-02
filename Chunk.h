#pragma once
#include <vector>
#include <map>
#include "VoxelTypes.h"
#include <string>
#include <unordered_map>
#include <utility>
#include <scene/resources/mesh.h>

namespace Voxel {
	struct Block;
	class World;
	class ChunkColumn;
	
	class Chunk {
		RID chunkActor;
		Ref<Mesh> chunkMesh;
	
		Transform3D transform;
		char chunkHeight;
	public:
		ChunkColumn* chunkColumn;
	
		World* world;
	
		Chunk(Vector3 chunkWorldPosition, World* worldParent, ChunkColumn* chunkColumnParent, char chunkHeightInColumn);
		
		// Aktualizuje mesh.
		void DrawChunk();
	
		const Block& GetBlockAt(const Vector3& position) const;
	
		// Funkcja zwraca sąsiedni chunk lub nullptr jeśli nie istnieje.
		const Chunk* GetNeighbour(BlockSide side) const;
		Chunk* GetNeighbour(BlockSide side);
	
		void DeleteObject();
	
		~Chunk();

	private:
		Chunk* GetNeighbourImpl(BlockSide side) const;
		void CreateActorIfEmpty();
		void UpdateMesh();
	};
}
