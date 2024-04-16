#pragma once
#include "VoxelTypes.h"
#include "Tools/Array3d.h"
#include "Chunk.h"
#include <queue>
#include <vector>
#include <memory>

namespace Voxel {
	class World;
	struct Block;
	
	class ChunkColumn {
	public:
		enum class ChunkStatus { GENERATED, DRAWN };

		ChunkColumn(Vector3 chunkColumnWorldPos, const World& worldParent, const int columnHeight);
	
		void SetBlock(const Vector3i &position, BlockID blockID);
		const Block &GetBlockAt(const Vector3i &position) const;

		void AddBlockModification(VoxelMod mod);
		void ApplyModifications();

		// Zmienia status na TO__DRAW.
		void AddChunksObjects();
		// Tworzy mesh i zmienia status na DRAWN.
		void DrawChunks();
		// Usuwa mesh i zmienia status na GENERATED.
		void DeleteChunksObjects();

		std::weak_ptr<ChunkColumn> GetNeighbour(BlockSide side) const;

		Chunk &GetChunk(int y) { return *chunks.at(y).get(); }
		const Chunk &GetChunk(int y) const { return *chunks.at(y).get(); }
		Array3d<Block> &GetBlockStorage() { return chunkBlocks; }
		const Array3d<Block> &GetBlockStorage() const { return chunkBlocks; }
		ChunkStatus GetStatus() const { return status; }
		bool GetToDraw() const { return toDraw; }

	private:
		ChunkStatus status = ChunkStatus::GENERATED;
		std::vector<std::unique_ptr<Chunk>> chunks;
		std::queue<VoxelMod> blocksModifications;
		Array3d<Block> chunkBlocks;
		bool toDraw = false;

		mutable std::weak_ptr<ChunkColumn> neighbours[4]{};
		const World &worldParent;
		const Vector3 columnPosInWorld;
		const int columnHeight;

		void GenerateChunks();
		void GenerateStructures();
	};
}
