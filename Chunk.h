#pragma once
#include <vector>
#include <map>
#include "VoxelTypes.h"
#include <string>
#include <unordered_map>
#include <utility>
#include <scene/resources/mesh.h>

namespace Voxel {
	class Block;
	class World;
	class ChunkColumn;
	
	struct MeshData {
		std::vector<Vector3> vertices;
		std::vector<Vector2> uvs;
		std::vector<int> textureIndexes;
		std::vector<int> triangles;
		std::vector<Vector3> normals;
		int vertexIndex;
	};
	
	class Chunk {
		RID chunkActor;
		Ref<ArrayMesh> chunkMesh;
	
		Transform3D transform;
		char chunkHeight;
	public:
		ChunkColumn* chunkColumn;
		
		std::unordered_map<std::string, MeshData> meshData;
	
		World* world;
	
		Chunk(Vector3 chunkWorldPosition, World* worldParent, ChunkColumn* chunkColumnParent, char chunkHeightInColumn);
		
		// Aktualizuje mesh.
		void DrawChunk();
	
		// Przygotowywuje do renderowania.
		void AddObject(Vector3 chunkWorldPos);
	
		void CombineBlockSides();
	
		Block& GetBlockAt(const Vector3& position);
	
		// Funkcja zwraca sąsiedni chunk lub nullptr jeśli nie istnieje.
		Chunk* GetNeighbour(BlockSide side);
	
		void DeleteObject();
	
		~Chunk();
	
	private:
		void ClearMeshData();
	};
}
