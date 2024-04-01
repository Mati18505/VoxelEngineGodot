#include "VoxelNode.h"
#include "VoxelTypes.h"
#include <fstream>
#include "Tools/TextParser.h"
#include "Tools/ActorManager.h"
#include "scene/main/viewport.h"
#include "scene/3d/camera_3d.h"
#include "BlockType.h"

#define ConvertYZ(vec3) Vector3(vec3.x, vec3.z, vec3.y)

void VoxelNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_materials"), &VoxelNode::get_materials);
	ClassDB::bind_method(D_METHOD("set_materials", "materials"), &VoxelNode::set_materials);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "materials"), "set_materials", "get_materials");

	ClassDB::bind_method(D_METHOD("get_textures"), &VoxelNode::get_textures);
	ClassDB::bind_method(D_METHOD("set_textures", "textures"), &VoxelNode::set_textures);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "textures"), "set_textures", "get_textures");
}


void VoxelNode::LoadWorldConfig() {
	Voxel::TextParser textP;
	Ref<FileAccess> configFile = FileAccess::open("res://Assets/config.txt", FileAccess::READ);
	String configText = configFile->get_as_text();
	textP.Parse(std::string(configText.utf8()));

	//World
	if (!textP.GetInt("renderDistance", world->renderDistance))
		print_line("config not contains renderDistance\n");
	if (!textP.GetInt("columnHeight", world->columnHeight))
		print_line("config not contains columnHeight\n");
	if (!textP.GetInt("seed", world->seed))
		print_line("config not contains seed\n");
	if (!textP.GetBool("useThreading", world->useThreading))
		print_line("config not contains useThreading\n");
	if (!textP.GetFloat("blockRayCastIncrement", world->blockRayCastIncrement))
		print_line("config not contains blockRayCastIncrement\n");
}


void VoxelNode::Ready() {
	if (inEditor)
		return;
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(2s);
	print_line("VoxelNode Ready");

	Voxel::ActorManager::Get().world = get_world_3d();

	actorManagerQueue.Clear();

	world = new Voxel::World(this);

	textureDictionary = std::make_unique<Voxel::TextureDictionary>(textures);
	materialDictionary = std::make_unique<Voxel::MaterialDictionary>(materials);

	LoadWorldConfig();

	// Read & parse blocks.json.
	Error error;
	String path = "res://Assets/blocks.json";
	path.simplify_path();
	Ref<FileAccess> texturesFile = FileAccess::open(path, FileAccess::READ, &error);
	if (error)
		throw 1;
	String texturesText = texturesFile->get_as_text();
	Voxel::Json texturesJson = Voxel::Json::parse(std::string(texturesText.utf8()));
	GenerateBlockTypes(texturesJson);


	Ref<FileAccess> biomesFile = FileAccess::open("res://Assets/biomes.json", FileAccess::READ);
	String biomesText = biomesFile->get_as_text();
	Voxel::Json biomesJson = Voxel::Json::parse(std::string(biomesText.utf8()));

	Camera3D* camera = GetCurrentCamera3D();
	if(camera)
	{
		Vector3 playerLocation = camera->get_global_position();
		world->Start(biomesJson, ConvertYZ(playerLocation));
	}
}

void VoxelNode::Process() {
	Camera3D *camera = GetCurrentCamera3D();
	if (camera)
	{
		Vector3 playerLocation = camera->get_global_position();
		world->Update(ConvertYZ(playerLocation));
	}

	actorManagerQueue.Resolve();
}

Camera3D* VoxelNode::GetCurrentCamera3D() {
	Camera3D *camera = nullptr;
	if (Viewport* viewport = get_viewport())
	{
		camera = viewport->get_camera_3d();
	}
	return camera;
}

VoxelNode::VoxelNode() {
#ifdef TOOLS_ENABLED
	inEditor = Engine::get_singleton()->is_editor_hint();
	//inEditor = false;
#endif
	if(!inEditor)
		set_process(true);
}


VoxelNode::~VoxelNode() {
	actorManagerQueue.Resolve();
	delete world;
	for (auto &blockType : blockTypes)
		delete blockType;
}


void VoxelNode::_notification(int p_what) {
	switch(p_what)
	{
		case NOTIFICATION_READY:
			Ready();
			break;
		case NOTIFICATION_PROCESS:
			Process();
			break;
	}
	
}

void VoxelNode::set_materials(const Dictionary materials) {
	this->materials = materials;
}

Dictionary VoxelNode::get_materials() const {
	return materials;
}


void VoxelNode::set_textures(Dictionary textures) {
	this->textures = textures;
}


Dictionary VoxelNode::get_textures() const {
	return textures;
}

void VoxelNode::GenerateBlockTypes(Voxel::Json jsonFile) {
	using namespace Voxel;

	for (int i = 0; i < jsonFile["blocks"].size(); i++) {
		Json blockP = jsonFile["blocks"][i];
		std::string materialName = blockP.contains("material") ? blockP["material"] : "default";
		bool isTransparent = blockP["isTransparent"] == "true";
		bool isTranslucent = blockP.contains("translucent") ? (bool)blockP["translucent"] : isTransparent;

		BlockType *block = new BlockType(blockP["name"], isTransparent, blockP["isEverySideSame"] == "true", materialName, isTranslucent);
		if (blockP["isEverySideSame"] == "true")
			block->SetBlockSideTextureIndex(textureDictionary->GetBlockTextureIndex(blockP["textures"]["side"]));
		else {
			block->SetBlockSideTextureIndex(textureDictionary->GetBlockTextureIndex(blockP["textures"]["side"]));
			block->SetBottomTextureIndex(textureDictionary->GetBlockTextureIndex(blockP["textures"]["bottom"]));
			block->SetTopTextureIndex(textureDictionary->GetBlockTextureIndex(blockP["textures"]["top"]));
		}
		blockTypes.push_back(block);
	}
}

Voxel::BlockID VoxelNode::GetBlockTypeIDFromName(const std::string& typeName) {
	using namespace Voxel;

	auto finded = std::find_if(blockTypes.begin(), blockTypes.end(), [&typeName](BlockType *blockType) { return blockType->name == typeName; });

	if (finded == blockTypes.end())
		throw std::invalid_argument("BlockType name does not exist!");

	return finded - blockTypes.begin();
}
