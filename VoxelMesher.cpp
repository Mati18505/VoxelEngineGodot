#include "VoxelMesher.h"
#include "BlockType.h"
#include "Block.h"
#include "VoxelNode.h"
#include "Tools/Profiler.h"

#define chunkSize gameMode.config.chunkSize
#define ConvertYZ(vec3) Vector3(vec3.x, vec3.z, vec3.y)

namespace Voxel {
	const int VoxelMesher::blockSideTriangles[6] = { 3, 1, 0, 3, 2, 1 };
	const Vector3 VoxelMesher::blockVertices[8] = { Vector3(-0.5f, -0.5f,  -0.5f),
	                                            Vector3(0.5f, -0.5f,  -0.5f),
	                                            Vector3(0.5f, -0.5f, 0.5f),
	                                            Vector3(-0.5f, -0.5f, 0.5f),
	                                            Vector3(-0.5f,  0.5f,  -0.5f),
	                                            Vector3(0.5f,  0.5f,  -0.5f),
	                                            Vector3(0.5f,  0.5f, 0.5f),
	                                            Vector3(-0.5f,  0.5f, 0.5f) };

	Ref<Mesh> VoxelMesher::CreateMesh(ChunkData chunkData) const
	{
		SM_PROFILE_ZONE;
		std::unordered_map<std::string, MeshData> materialsMeshData;
	
		for (int z = 0; z < chunkSize; z++)
		{
			for (int y = 0; y < chunkSize; y++)
			{
				for (int x = 0; x < chunkSize; x++)
				{
					const Block &block = chunkData.voxelData.At({ x, y, z + chunkData.chunkHeightInColumn });
					const auto& blockType = gameMode.blockTypes[block.typeID];
					MeshData& blockMeshData = materialsMeshData[blockType.materialName];
	
					CreateBlock(block.typeID, Vector3(x, y, z), blockMeshData, chunkData);
				}
			}
		}
		
		return CombineMeshes(materialsMeshData);
	}

	template<typename T>
	static Vector<T> ConvertToGodotVector(const std::vector<T>& v)
	{
		Vector<T> result;
		result.resize(v.size());

		size_t i = 0;
		for (const T& e : v)
			result.set(i++, e);

		return result;
	}

	Ref<Mesh> VoxelMesher::CombineMeshes(const std::unordered_map<std::string, MeshData> &materialsMeshData) const {
		SM_PROFILE_ZONE;
		Ref<ArrayMesh> mesh = Ref<ArrayMesh>(memnew(ArrayMesh));

		int it = 0;
		for (auto &pair : materialsMeshData)
		{
			const std::string& name = pair.first;
			const MeshData& data = pair.second;
			if (data.vertexIndex == 0)
				continue;

			// Konwertowanie danych z kontenerów std do kontenerów godot.
			Vector<Vector3> vertices = ConvertToGodotVector(data.vertices);
			Vector<int> triangles = ConvertToGodotVector(data.triangles);
			Vector<Vector2> uvs = ConvertToGodotVector(data.uvs);
			Vector<Vector3> normals = ConvertToGodotVector(data.normals);
			Vector<Vector2> uvs2;
			uvs2.resize(data.textureIndexes.size());

			size_t i = 0;
			for (int textureIndex : data.textureIndexes)
				uvs2.set(i++, { (float)textureIndex, 0 });
	
			Array surfaceArray;
			surfaceArray.resize(Mesh::ARRAY_MAX);
			surfaceArray[Mesh::ARRAY_VERTEX] = vertices;
			surfaceArray[Mesh::ARRAY_INDEX] = triangles;
			surfaceArray[Mesh::ARRAY_TEX_UV] = uvs;
			surfaceArray[Mesh::ARRAY_TEX_UV2] = uvs2;
			surfaceArray[Mesh::ARRAY_NORMAL] = normals;
			mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surfaceArray);
	
			mesh->surface_set_material(it, gameMode.materialDictionary.GetMaterialFromName(name));
	
			it++;
		}

		return mesh;
	}

	void VoxelMesher::CreateBlockBlockSide(const BlockID type, BlockSide side, const Vector3 &posInChunk, MeshData &meshData) const {
		const BlockType& myType = gameMode.blockTypes[type];
	    // TODO: optymalizacja: vertices.reserve(x) w chunk: drawchunk
	    switch (side)
	    {
	    case BACK:
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[4]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[5]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[1]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[0]);
	        break;
		case FRONT:
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[6]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[7]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[3]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[2]);
	        break;
	    case LEFT:
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[7]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[4]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[0]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[3]);
	        break;
	    case RIGHT:
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[5]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[6]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[2]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[1]);
	        break;
	    case TOP:
	        meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[7]);
	        meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[6]);
	        meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[5]);
	        meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[4]);
	        break;
	    case BOTTOM:
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[0]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[1]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[2]);
			meshData.vertices.push_back(ConvertYZ(posInChunk) + blockVertices[3]);
	        break;
	    }
		for (int i = 0; i < 4; i++)
			meshData.normals.push_back(ConvertYZ(GetVectorFromBlockSide(side)));

	    meshData.uvs.push_back(Vector2(1.f, 0.f));
	    meshData.uvs.push_back(Vector2(0.f, 0.f));
	    meshData.uvs.push_back(Vector2(0.f, 1.f));
	    meshData.uvs.push_back(Vector2(1.f, 1.f));
	
	    for (int i = 0; i < 4; i++)
	        meshData.textureIndexes.push_back(myType.GetBlockSideTextureIndex(side));
	
	    for (int i = 0; i < 6; i++)
	        meshData.triangles.push_back(meshData.vertexIndex + blockSideTriangles[i]);
	    meshData.vertexIndex += 4;
	}
	
	bool VoxelMesher::HasTransparentNeighbour(BlockSide side, const Vector3 &posInChunk, const ChunkData &chunkData) const
	{
		Vector3 neighbourPosition = posInChunk + GetVectorFromBlockSide(side);
	
	    if (neighbourPosition.x < 0 || neighbourPosition.x >= chunkSize ||
	        neighbourPosition.y < 0 || neighbourPosition.y >= chunkSize ||
	        neighbourPosition.z < 0 || neighbourPosition.z >= chunkSize)
	    {
			return true;
	        /*const Chunk* neighbourChunk = chunkParent.GetNeighbour(side);
	        if (neighbourChunk == nullptr)
	            return true;
	
	        chunk = neighbourChunk;
	        neighbourPosition -= chunkSize * GetVectorFromBlockSide(side);*/
	    }

		neighbourPosition.z += chunkData.chunkHeightInColumn;
		const BlockID neighbourID = chunkData.voxelData[neighbourPosition].typeID;
		return gameMode.blockTypes[neighbourID].isTranslucent;
	   // return true;  
	}
	
	void VoxelMesher::CreateBlock(const BlockID type, const Vector3 &posInDrawChunk, MeshData &meshData, const ChunkData &chunkData) const
	{
		if (gameMode.blockTypes[type].isTransparent)
	        return;
	
	    for (int i = 0; i < 6; i++)
	    {
			if (HasTransparentNeighbour((BlockSide)i, posInDrawChunk, chunkData))
	            CreateBlockBlockSide(type, (BlockSide)i, posInDrawChunk, meshData);
	    }
	}
}
