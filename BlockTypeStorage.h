#pragma once
#include <vector>
#include "vendor/tcbrindle/span.h"
#include "VoxelTypes.h"
#include "BlockType.h"

namespace Voxel {
	class BlockTypeStorage {
	public:
		BlockTypeStorage() = default;
		BlockTypeStorage(std::vector<BlockType> blockTypes)
			: blockTypes(std::move(blockTypes))
		{}
		BlockID GetBlockTypeIDFromName(const std::string &typeName) const;

		tcb::span<const BlockType> Data() const {
			return tcb::span{ blockTypes.data(), blockTypes.size() };
		}

		operator tcb::span<const BlockType>() const {
			return Data();
		}
		const BlockType& operator[](size_t i) const {
			return blockTypes[i];
		}
	private:
		std::vector<BlockType> blockTypes;
	};
}
