#pragma once
#include <iostream>
#include <vector>
#include "VoxelTypes.h"

namespace Voxel {
	class Chunk;
	struct MeshData;
	
	class Block {
	private:
	    static const int sideTriangles[6];
	    static const Vector3 vertices[8];
	
	    BlockID typeID;
	    
	public:
	    Block(BlockID typeID);
	    Block();
	
	    /// <summary>
	    /// Tworzy mesh bloku
	    /// </summary>
	    /// <returns>Mesh bloku</returns>
	    void CreateBlock(Chunk& chunkParent, const Vector3& posInDrawChunk, MeshData& meshData);
	
	
	    /// <summary>
	    /// Tworzy mesh ściany
	    /// </summary>
	    /// <param name="side">Ściana której mesh ma stworzyć</param>
	    /// <returns>Mesh ściany</returns>
	    void CreateBlockBlockSide(BlockSide side, Chunk& chunkParent, const Vector3& posInDrawChunk, MeshData& meshData);
	    
	    bool HasTransparentNeighbour(BlockSide side, Chunk& chunkParent, const Vector3& posInDrawChunk);
	
	    BlockID GetBlockTypeID() { return typeID; }
	
	private:
	    Vector3 GetVectorFromBlockSide(BlockSide side);
	};
}
