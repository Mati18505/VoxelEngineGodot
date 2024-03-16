#ifndef VOXELNODE_H
#define VOXELNODE_H

#include <core/object/ref_counted.h>
#include <scene/resources/material.h>
#include <scene/resources/image_texture.h>
#include <scene/3d/node_3d.h>
#include "World.h"
#include "Tools/ActorManagerQueue.h"

class VoxelNode : public Node3D
{
	GDCLASS(VoxelNode, Node3D);

public:
	VoxelNode();
	~VoxelNode();

	Dictionary materials;
	Ref<Texture2DArray> textureArray;
	
	Voxel::World *world = nullptr;
	Voxel::ActorManagerQueue actorManagerQueue;

	void _notification(int p_what);

	void set_materials(const Dictionary materials);
	Dictionary get_materials() const;

	void set_texture_array(const Ref<Texture2DArray> textureArray);
	Ref<Texture2DArray> get_texture_array() const;

protected:
	static void _bind_methods();

private:
	void LoadWorldConfig();
	void Ready();
	void Process();
	bool inEditor = false;
};
#endif // VOXELNODE_H
