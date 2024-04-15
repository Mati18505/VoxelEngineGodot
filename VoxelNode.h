#ifndef VOXELNODE_H
#define VOXELNODE_H

#include <core/object/ref_counted.h>
#include <scene/resources/material.h>
#include <scene/resources/image_texture.h>
#include <scene/3d/node_3d.h>
#include "GameMode.h"
#include <optional>

class VoxelNode : public Node3D
{
	GDCLASS(VoxelNode, Node3D);

public:
	VoxelNode();

	Dictionary materials{};
	Dictionary textures{};

	void _notification(int p_what);

	void set_materials(const Dictionary materials);
	Dictionary get_materials() const;

	void set_textures(const Dictionary textures);
	Dictionary get_textures() const;

	bool GetInEditor() const { return inEditor; }

	std::unique_ptr<Voxel::GameMode> gameMode;

protected:
	static void _bind_methods();

private:
	void Ready();
	void Process();
	std::optional<std::string> ReadFile(std::string_view file) const;
	Voxel::GameMode::Config LoadWorldConfig() const;
	Vector3 GetPlayerPos() const;
	Camera3D *GetCurrentCamera3D() const;

	bool inEditor = false;
};
#endif // VOXELNODE_H
