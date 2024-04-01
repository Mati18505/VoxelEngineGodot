#pragma once
#include <core/math/vector3.h>
#include <core/math/vector3i.h>
#include <core/math/vector2.h>
#include <core/math/vector2i.h>
#include <stdexcept>

namespace Voxel {
	using BlockID = unsigned char;
	enum BlockSide { FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM };
	
	struct VoxelMod {
		BlockID id;
		Vector3i pos;
	};

	static Vector3 GetVectorFromBlockSide(BlockSide side)
	{
	    switch (side)
	    {
	    case FRONT:
	        return { 0, 1, 0 };
	        break;
	    case BACK:
	        return { 0, -1, 0 };
	        break;
	    case LEFT:
	        return { -1, 0, 0 };
	        break;
	    case RIGHT:
	        return { 1, 0, 0 };
	        break;
	    case TOP:
	        return { 0, 0, 1 };
	        break;
	    case BOTTOM:
	        return { 0, 0, -1 };
	        break;
	    default:
	        throw std::invalid_argument("Invalid side!");
	        break;
	    }
	}
}
