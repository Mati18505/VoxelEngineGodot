#pragma once
#include "BlockType.h"
#include <map>
#include <scene/resources/material.h>
#include <core/variant/dictionary.h>
#include <string>

namespace Voxel {
	class MaterialDictionary {
public:
		MaterialDictionary(const Dictionary &materials);

		Ref<Material> GetBlockTypeMaterial(const BlockType &blockType) const;
		Ref<Material> GetMaterialFromName(const std::string &name) const;

	private:
		std::map<std::string, Ref<Material>> materialDictionary;
		void GenerateMaterialDictionary(const Dictionary &materials);
	};
}
