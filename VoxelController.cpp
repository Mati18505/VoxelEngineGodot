#include "VoxelController.h"
#include "VoxelNode.h"
#include "scene/main/viewport.h"
#include "scene/3d/camera_3d.h"

#define ConvertYZ(vec3) Vector3(vec3.x, vec3.z, vec3.y)

VoxelController::VoxelController() {
#ifdef TOOLS_ENABLED
	inEditor = Engine::get_singleton()->is_editor_hint();
	//inEditor = false;
#endif
	if(!inEditor)
		set_process(true);
}

void VoxelController::_notification(int p_what) {
	switch(p_what)
	{
		case NOTIFICATION_READY:
			Ready();
			break;
		case NOTIFICATION_PROCESS:
			Process();
			break;
	}
}

void VoxelController::_bind_methods() {
	ClassDB::bind_method(D_METHOD("DestroyBlock"), &VoxelController::DestroyBlock);
	ClassDB::bind_method(D_METHOD("PlaceBlock"), &VoxelController::PlaceBlock);
}

void VoxelController::Ready() {
	if(!inEditor)
		gameMode = dynamic_cast<VoxelNode *>(get_node(NodePath("/root/Main/VoxelNode")));
}

void VoxelController::Process() {
}

void VoxelController::DestroyBlock() {
	Camera3D* camera = GetCurrentCamera3D();
	Vector3 direction = -camera->get_global_transform().basis.get_column(2);
	Vector3 startPoint = camera->get_global_transform().origin;
	Vector3 blockPos;
	if (gameMode->world->BlockRayCast(ConvertYZ(startPoint), ConvertYZ(direction), &blockPos))
	{
		gameMode->world->SetBlock(blockPos, 0);
	}
}

void VoxelController::PlaceBlock()
{
	Camera3D* camera = GetCurrentCamera3D();
	Vector3 direction = -camera->get_global_transform().basis.get_column(2);
	Vector3 startPoint = camera->get_global_transform().origin;
	Vector3 blockPos;
	Vector3 blockPlacePos;
	if (gameMode->world->BlockRayCast(ConvertYZ(startPoint), ConvertYZ(direction), &blockPos, 100.f, &blockPlacePos))
	{
		gameMode->world->SetBlock(blockPlacePos, 2);
	}
}

Camera3D *VoxelController::GetCurrentCamera3D() {
	Camera3D *camera = nullptr;
	if (Viewport* viewport = get_viewport())
	{
		camera = viewport->get_camera_3d();
	}
	return camera;
}
