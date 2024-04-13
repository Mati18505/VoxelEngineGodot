#include "MaterialDictionary.h"
#include "Tools/Profiler.h"

namespace Voxel
{
	MaterialDictionary::MaterialDictionary(const Dictionary &materials) {
		GenerateMaterialDictionary(materials);
	}
	
	Ref<Material> MaterialDictionary::GetBlockTypeMaterial(BlockType *blockType) {
		return materialDictionary.at(blockType->GetMaterialName());
	}
	
	Ref<Material> MaterialDictionary::GetMaterialFromName(const std::string &name) {
		return materialDictionary.at(name);
	}
	
	void MaterialDictionary::GenerateMaterialDictionary(const Dictionary &materials) {
		SM_PROFILE_ZONE;
		for (int i = 0; i < materials.size(); i++) {
			const Variant &key = materials.get_key_at_index(i);
			const Variant &value = materials.get_value_at_index(i);
			String materialName = key;
			Ref<Material> material = value;
	
			materialDictionary.emplace(materialName.utf8(), material);
		}
	}
}
