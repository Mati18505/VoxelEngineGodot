#include "register_types.h"

#include "core/object/class_db.h"
#include "VoxelNode.h"

void initialize_voxel_module(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		return;
	}
	ClassDB::register_class<VoxelNode>();
}

void uninitialize_voxel_module(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		return;
	}
}
