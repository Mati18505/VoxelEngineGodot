#pragma once
#include <scene/3d/node_3d.h>

class VoxelNode;
class VoxelController : public Node3D {
	GDCLASS(VoxelController, Node3D);

public:
	VoxelController();
	VoxelNode *gameMode;

	void _notification(int p_what);

protected:
	static void _bind_methods();

private:
	void Ready();
	void Process();

	void DestroyBlock();
	void PlaceBlock();
	Camera3D* GetCurrentCamera3D();
	bool inEditor;
};
