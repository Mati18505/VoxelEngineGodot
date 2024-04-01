#pragma once
#include "VoxelTypes.h"
#include "Tools/Array3d.h"
#include <queue>

namespace Voxel {
	class World;
	class Chunk;
	struct Block;
	
	class ChunkColumn {
		Vector3 columnPosInWorld;
		World* worldParent;
		ChunkColumn* neighbours[4] {};
	
		int columnHeight;
		void GenerateChunks();
		void GenerateStructures();
	public:
		enum class ChunkStatus { GENERATED, DRAWN };
		ChunkStatus status;
		bool toDraw = false;
	
		Array3d<Block> chunkBlocks;
		std::queue<VoxelMod> blocksModifications;
	
		Chunk** chunks;
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
	
		// Funkcja zwraca sąsiedni chunk lub nullptr jeśli nie istnieje.
		ChunkColumn* GetNeighbour(BlockSide side);
	
		~ChunkColumn();
	};
}
