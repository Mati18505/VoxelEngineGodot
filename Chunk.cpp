#pragma once 
#include "Chunk.h"

#include "Tools/ActorManager.h"

#include "Block.h"
#include "World.h"
#include "ChunkColumn.h"
#include "BlockType.h"
#include "VoxelNode.h"

#define chunkSize world->chunkSize
#define chunkScaledSize world->chunkScaledSize
#define Array3D(x, y, z, width, height) ((y) * width * height) + ((z) * width) + x
#define Chunk3DArray(x, y, z) Array3D(x, y, z, chunkSize, chunkSize * world->columnHeight)

#define ConvertYZ(vec3) Vector3(vec3.x, vec3.z, vec3.y)

namespace Voxel {
	Chunk::Chunk(Vector3 chunkWorldPosition, World* worldParent, ChunkColumn* chunkColumnParent, char chunkHeight)
	{
		world = worldParent;
		transform = Transform3D();
		transform.origin = ConvertYZ(chunkWorldPosition);
		chunkColumn = chunkColumnParent;
		this->chunkHeight = chunkHeight;
		chunkMesh = Ref<ArrayMesh>(memnew(ArrayMesh));
	}
	
	void Chunk::CombineBlockSides() {
		int it = 0;
		for (auto &pair : meshData)
		{
			const std::string& name = pair.first;
			MeshData& data = pair.second;
			// Konwertowanie danych z kontenerów std do kontenerów godot.
			Vector<Vector3> vertices;
			Vector<int> triangles;
			Vector<Vector2> uvs;
			Vector<Vector2> uvs2;
			Vector<Vector3> normals;
	
			for (Vector3 i : data.vertices)
				vertices.push_back(i);
			for (int i : data.triangles)
				triangles.push_back(i);
			for (Vector2 i : data.uvs)
				uvs.push_back(i);
			for (Vector3 i : data.normals)
				normals.push_back(i);
			for (int textureIndex : data.textureIndexes)
				uvs2.push_back({ (float)textureIndex, 0 });
	
			Array surfaceArray;
			surfaceArray.resize(Mesh::ARRAY_MAX);
			surfaceArray[Mesh::ARRAY_VERTEX] = vertices;
			surfaceArray[Mesh::ARRAY_INDEX] = triangles;
			surfaceArray[Mesh::ARRAY_TEX_UV] = uvs;
			surfaceArray[Mesh::ARRAY_TEX_UV2] = uvs2;
			surfaceArray[Mesh::ARRAY_NORMAL] = normals;
			chunkMesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surfaceArray);
	
			auto& nameCapture = name;
			chunkMesh->surface_set_material(it, world->GetMaterialFromName(nameCapture));
	
			it++;
		}
	}
	
	void Chunk::DrawChunk() {
		ClearMeshData();
	
		for (int z = 0; z < chunkSize; z++)
		{
			for (int y = 0; y < chunkSize; y++)
			{
				for (int x = 0; x < chunkSize; x++)
				{
					Block& block = chunkColumn->chunkBlocks.At({ x, y, z + this->chunkHeight });
					auto blockType = world->blockTypes.at(block.GetBlockTypeID());
					MeshData& blockMeshData = meshData[blockType->GetMaterialName()];
	
					block.CreateBlock(*this, Vector3(x, y, z), blockMeshData);
				}
			}
		}
		CombineBlockSides();
	
		ClearMeshData();
	}
	
	void Chunk::AddObject(Vector3 chunkWorldPos) {
		transform.origin = ConvertYZ(chunkWorldPos);
		Vector3 scale(world->worldScale, world->worldScale, world->worldScale);
		transform.scale(scale);
	
		world->gameMode->actorManagerQueue.AddFunction([=]() {
			ActorManager::Get().CreateActor(chunkActor, chunkMesh, transform);
		});
	}
	
	Block& Chunk::GetBlockAt(const Vector3& position) {
		int posZ = int(position.z) + chunkHeight;
	
		return chunkColumn->chunkBlocks.At({ int(position.x), int(position.y), int(posZ) });
	}
	
	Chunk* Chunk::GetNeighbour(BlockSide side)
	{
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
	
		if (neighbourChunkIndex < 0 || neighbourChunkIndex >= world->columnHeight)
			return nullptr;
	
		return chunkColumn->chunks[neighbourChunkIndex];
	}
	
	void Chunk::DeleteObject()
	{
		world->gameMode->actorManagerQueue.AddFunction([=]() {
			ActorManager::Get().DestroyActor(chunkActor);
		});
	
		ClearMeshData();
	}
	
	void Chunk::ClearMeshData()
	{
		for (auto &pair : meshData)
		{
			MeshData &data = pair.second;
			data.vertices.clear();
			data.normals.clear();
			data.uvs.clear();
			data.textureIndexes.clear();
			data.triangles.clear();
			data.vertexIndex = 0;
		}
	}
	
	Chunk::~Chunk()
	{
		DeleteObject();
	}
}
