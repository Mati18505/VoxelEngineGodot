#ifndef VOXELNODE_H
#define VOXELNODE_H

#include <core/object/ref_counted.h>
#include <scene/resources/material.h>
#include <scene/resources/image_texture.h>
#include <scene/3d/node_3d.h>
#include "World.h"
#include "Tools/ActorManagerQueue.h"
#include "TextureDictionary.h"
#include "MaterialDictionary.h"
#include "VoxelTypes.h"
#include "BlockTypeStorage.h"

namespace Voxel {
class VoxelMesher;
}

class VoxelNode : public Node3D
{
	GDCLASS(VoxelNode, Node3D);

public:
	VoxelNode();
	~VoxelNode();

	Dictionary materials;
	Dictionary textures;
	
	Voxel::World *world = nullptr;
	Voxel::ActorManagerQueue actorManagerQueue;
	std::unique_ptr<Voxel::VoxelMesher> voxelMesher;

	std::unique_ptr<Voxel::TextureDictionary> textureDictionary;
	std::unique_ptr<Voxel::MaterialDictionary> materialDictionary;
	Voxel::BlockTypeStorage blockTypes;

	void _notification(int p_what);

	void set_materials(const Dictionary materials);
	Dictionary get_materials() const;

	void set_textures(const Dictionary textures);
	Dictionary get_textures() const;

protected:
	static void _bind_methods();

private:
	void GenerateBlockTypes(Voxel::Json jsonFile);
	void LoadWorldConfig();
	void Ready();
	void Process();
	Camera3D* GetCurrentCamera3D();
	bool inEditor = false;
	Ref<Texture2DArray> textureArray;
};
#endif // VOXELNODE_H
