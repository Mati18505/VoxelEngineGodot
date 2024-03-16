#pragma once

namespace Voxel {
	namespace VoxelMath {
	
		static float MapValue(float from, float to, float from2, float to2, float value) {
			if (value <= from2)
				return from;
			if (value >= to2)
				return to;
			return (to - from) * ((value - from2) / (to2 - from2)) + from;
		}
	}
}
