#pragma once
#include <scene/resources/mesh.h>
#include "Tools/Array3d.h"
#include "Chunk.h"
#include "GameMode.h"

class VoxelNode;

namespace Voxel {
	class VoxelMesher {
	public:
		VoxelMesher(const GameMode &gameMode) :
			gameMode(gameMode)
		{}
		struct ChunkData {
			const Array3d<Block> voxelData;
			uint8_t chunkHeightInColumn;
		};

		Ref<Mesh> CreateMesh(ChunkData chunkData) const;

	private:
		struct MeshData {
			std::vector<Vector3> vertices;
			std::vector<Vector2> uvs;
			std::vector<int> textureIndexes;
			std::vector<int> triangles;
			std::vector<Vector3> normals;
			int vertexIndex;
		};

		Ref<Mesh> CombineMeshes(const std::unordered_map<std::string, MeshData>& materialsMeshData) const;

		static const int blockSideTriangles[6];
	    static const Vector3 blockVertices[8];

		void CreateBlock(const BlockID type, const Vector3 &posInDrawChunk, MeshData &meshData, const ChunkData &chunkData) const;
		void CreateBlockBlockSide(const BlockID type, BlockSide side, const Vector3 &posInDrawChunka, MeshData &meshData) const;
		bool HasTransparentNeighbour(BlockSide side, const Vector3 &posInDrawChunk, const ChunkData &chunkData) const;

		const GameMode &gameMode;
	};
}
