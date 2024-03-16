#pragma once
#include <core/math/vector3.h>
#include <core/math/vector3i.h>
#include <core/math/vector2.h>
#include <core/math/vector2i.h>

namespace Voxel {
	using BlockID = unsigned char;
	enum BlockSide { FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM };
	
	struct VoxelMod {
		BlockID id;
		Vector3i pos;
	};
}
