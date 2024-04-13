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

		Ref<Material> GetBlockTypeMaterial(const BlockType& blockType);
		Ref<Material> GetMaterialFromName(const std::string& name);

	private:
		std::map<std::string, Ref<Material>> materialDictionary;
		void GenerateMaterialDictionary(const Dictionary &materials);
	};
}
