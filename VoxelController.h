#pragma once
#include <scene/3d/node_3d.h>

class VoxelNode;
class VoxelController : public Node3D {
	GDCLASS(VoxelController, Node3D);

public:
	VoxelController();
	VoxelNode *voxelNode;

	void _notification(int p_what);

protected:
	static void _bind_methods();

private:
	void Ready();
	void Process();

	void DestroyBlock() const;
	void PlaceBlock() const;
	Camera3D *GetCurrentCamera3D() const;

	bool inEditor;
};
