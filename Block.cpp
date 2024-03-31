#pragma once
#include "Block.h"

#include "Chunk.h"
#include "World.h"
#include "BlockType.h"
#include "ChunkColumn.h"
#include "VoxelNode.h"
#define chunkSize chunkParent.world->chunkSize
#define ConvertYZ(vec3) Vector3(vec3.x, vec3.z, vec3.y)

namespace Voxel
{
	// Unreal Engine: z=y y=z: triangles, vertices, normals
	const int Block::sideTriangles[6] = { 3, 1, 0, 3, 2, 1 };
	const Vector3 Block::vertices[8] = { Vector3(-0.5f, -0.5f,  -0.5f),
	                                            Vector3(0.5f, -0.5f,  -0.5f),
	                                            Vector3(0.5f, -0.5f, 0.5f),
	                                            Vector3(-0.5f, -0.5f, 0.5f),
	                                            Vector3(-0.5f,  0.5f,  -0.5f),
	                                            Vector3(0.5f,  0.5f,  -0.5f),
	                                            Vector3(0.5f,  0.5f, 0.5f),
	                                            Vector3(-0.5f,  0.5f, 0.5f) };
	
	Block::Block(BlockID typeID)
	{
	    this->typeID = typeID;
	}
	
	Block::Block()
	{
	    this->typeID = 0;
	}
	
	void Block::CreateBlockBlockSide(BlockSide side, Chunk& chunkParent, const Vector3& posInChunk, MeshData& meshData) {
	    BlockType* myType = chunkParent.world->gameMode->blockTypes[typeID];
	    // TODO: optymalizacja: vertices.reserve(x) w chunk: drawchunk
	    switch (side)
	    {
	    case FRONT:
				meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[4]);
				meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[5]);
				meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[1]);
				meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[0]);
	        for (int i = 0; i < 4; i++)
	            meshData.normals.push_back(Vector3(0.f, 0.f, -1.f));
	        break;
	    case BACK:
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[6]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[7]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[3]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[2]);
	        for (int i = 0; i < 4; i++)
	            meshData.normals.push_back(Vector3(0.f, 0.f, 1.f));
	        break;
	    case LEFT:
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[7]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[4]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[0]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[3]);
	        for (int i = 0; i < 4; i++)
	            meshData.normals.push_back(Vector3(-1.f, 0.f, 0.f));
	        break;
	    case RIGHT:
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[5]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[6]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[2]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[1]);
	        for (int i = 0; i < 4; i++)
	            meshData.normals.push_back(Vector3(1.f, 0.f, 0.f));
	        break;
	    case TOP:
	        meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[7]);
	        meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[6]);
	        meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[5]);
	        meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[4]);
	        for (int i = 0; i < 4; i++)
	            meshData.normals.push_back(Vector3(0.f, 1.f, 1.f));
	        break;
	    case BOTTOM:
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[0]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[1]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[2]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + vertices[3]);
	        for (int i = 0; i < 4; i++)
	            meshData.normals.push_back(Vector3(0.f, -1.f, -1.f));
	        break;
	    }
	    meshData.uvs.push_back(Vector2(1.f, 0.f));
	    meshData.uvs.push_back(Vector2(0.f, 0.f));
	    meshData.uvs.push_back(Vector2(0.f, 1.f));
	    meshData.uvs.push_back(Vector2(1.f, 1.f));
	
	    for (int i = 0; i < 4; i++)
	        meshData.textureIndexes.push_back(myType->GetBlockSideTextureIndex(side));
	
	    for (int i = 0; i < 6; i++)
	        meshData.triangles.push_back(meshData.vertexIndex + sideTriangles[i]);
	    meshData.vertexIndex += 4;
	
	}
	
	bool Block::HasTransparentNeighbour(BlockSide side, Chunk& chunkParent, const Vector3& posInChunk) 
	{
	    Vector3 neighbourPosition = posInChunk + GetVectorFromBlockSide(side);
	    Chunk* chunkColumn = &chunkParent;
	
	    if (neighbourPosition.x < 0 || neighbourPosition.x >= chunkSize || //Jeśli sąsiad wychodzi poza chunka TODO: sprawdzać sąsiednie chunki
	        neighbourPosition.y < 0 || neighbourPosition.y >= chunkSize/* * this->chunkParent->world->columnHeight */ ||
	        neighbourPosition.z < 0 || neighbourPosition.z >= chunkSize)
	    {
	        Chunk* neighbourChunk = chunkParent.GetNeighbour(side);
	        if (neighbourChunk == nullptr)
	            return true;
	
	        chunkColumn = neighbourChunk;
	        neighbourPosition -= chunkSize * GetVectorFromBlockSide(side);
	    }
	    
	    BlockID neighbourID = chunkColumn->GetBlockAt(neighbourPosition).typeID;
		return chunkParent.world->gameMode->blockTypes[neighbourID]->GetIsTranslucent();
	   // return true;  
	}
	
	void Block::CreateBlock(Chunk& chunkParent, const Vector3& posInDrawChunk, MeshData& meshData)
	{
		if (chunkParent.world->gameMode->blockTypes[typeID]->GetIsTransparent())
	        return;
	
	    for (int i = 0; i < 6; i++)
	    {
	        if (HasTransparentNeighbour((BlockSide)i, chunkParent, posInDrawChunk))
	            CreateBlockBlockSide((BlockSide)i, chunkParent, posInDrawChunk, meshData);
	    }
	}
	
	Vector3 Block::GetVectorFromBlockSide(BlockSide side)
	{
	    switch (side)
	    {
	    case FRONT:
	        return { 0, 1, 0 };
	        break;
	    case BACK:
	        return { 0, -1, 0 };
	        break;
	    case LEFT:
	        return { -1, 0, 0 };
	        break;
	    case RIGHT:
	        return { 1, 0, 0 };
	        break;
	    case TOP:
	        return { 0, 0, 1 };
	        break;
	    case BOTTOM:
	        return { 0, 0, -1 };
	        break;
	    default:
	        throw std::invalid_argument("Invalid side!");
	        break;
	    }
	}
}
