#pragma once
#include "VoxelTypes.h"
#include "Tools/Array3d.h"
#include "Chunk.h"
#include <queue>
#include <vector>
#include <memory>

namespace Voxel {
	class World;
	class Chunk;
	struct Block;
	
	class ChunkColumn {
	public:
		enum class ChunkStatus { GENERATED, DRAWN };
		ChunkStatus status;
		bool toDraw = false;
	
		Array3d<Block> chunkBlocks;
		std::queue<VoxelMod> blocksModifications;
	
		std::vector<std::unique_ptr<Chunk>> chunks;
		ChunkColumn(Vector3 chunkColumnWorldPos, World* worldParent, const int columnHeight);
	
		void SetBlock(Vector3i position, BlockID blockID);
		void ApplyModifiations();
		Block& GetBlockAt(Vector3i position);
		void DrawChunks();
	
		/// <summary>
		/// Odwrotność DeleteChunksObjects, zmienia status na TO__DRAW
		/// </summary>
		/// <param name="chunkColumnPos"></param>
		void AddChunksObjects(Vector3 chunkColumnWorldPos);
		/// <summary>
		/// Usuwa rzeczy związane z renderowaniem chunków i zmienia status na GENERATED
		/// </summary>
		void DeleteChunksObjects();
	
		std::weak_ptr<ChunkColumn> GetNeighbour(BlockSide side);

	private:
		Vector3 columnPosInWorld;
		World *worldParent;
		std::weak_ptr<ChunkColumn> neighbours[4]{};

		int columnHeight;
		void GenerateChunks();
		void GenerateStructures();
	};
}
