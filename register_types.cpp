#include "register_types.h"

#include "core/object/class_db.h"
#include "VoxelNode.h"
#include "VoxelController.h"

void initialize_voxel_module(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		return;
	}
	ClassDB::register_class<VoxelNode>();
	ClassDB::register_class<VoxelController>();
}

void uninitialize_voxel_module(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		return;
	}
}
