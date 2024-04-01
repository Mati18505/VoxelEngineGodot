#pragma once
#include "VoxelTypes.h"

namespace Voxel {
	struct Block {
	    BlockID typeID = 0;

		Block() = default;
		Block(BlockID type)
			: typeID(type)
		{}
	};
}
