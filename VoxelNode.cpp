#include "VoxelNode.h"
#include "VoxelTypes.h"
#include <fstream>
#include "Tools/TextParser.h"
#include "Tools/ActorManager.h"
#include "scene/main/viewport.h"
#include "scene/3d/camera_3d.h"
#include "BlockType.h"
#include "VoxelMesher.h"
#include "Tools/ActorManager.h"
#include "Tools/Profiler.h"

#define ConvertYZ(vec3) Vector3(vec3.x, vec3.z, vec3.y)

void VoxelNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_materials"), &VoxelNode::get_materials);
	ClassDB::bind_method(D_METHOD("set_materials", "materials"), &VoxelNode::set_materials);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "materials"), "set_materials", "get_materials");

	ClassDB::bind_method(D_METHOD("get_textures"), &VoxelNode::get_textures);
	ClassDB::bind_method(D_METHOD("set_textures", "textures"), &VoxelNode::set_textures);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "textures"), "set_textures", "get_textures");
}


Voxel::GameMode::Config VoxelNode::LoadWorldConfig() const {
	SM_PROFILE_ZONE;
	Voxel::TextParser textP;
	textP.Parse(ReadFile("res://Assets/config.txt").value_or(""));

	Voxel::GameMode::Config config;

	if (!textP.GetInt("renderDistance",config.renderDistance))
		print_line("config not contains renderDistance\n");
	if (!textP.GetInt("columnHeight", config.columnHeight))
		print_line("config not contains columnHeight\n");
	if (!textP.GetInt("seed", config.seed))
		print_line("config not contains seed\n");
	if (!textP.GetBool("useThreading", config.useThreading))
		print_line("config not contains useThreading\n");
	if (!textP.GetFloat("blockRayCastIncrement", config.blockRayCastIncrement))
		print_line("config not contains blockRayCastIncrement\n");

	return config;
}


void VoxelNode::Ready() {
	SM_PROFILE_ZONE;
	if (inEditor)
		return;
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(2s);
	print_line("VoxelNode Ready");

	Voxel::ActorManager::Get().world = get_world_3d();

	std::string biomesText = ReadFile("res://Assets/biomes.json").value_or("");
	nlohmann::json biomesJson = nlohmann::json::parse(biomesText);
	nlohmann::json blocksJson = nlohmann::json::parse(ReadFile("res://Assets/blocks.json").value_or(""));

	gameMode = std::make_unique<Voxel::GameMode>(*this, std::move(LoadWorldConfig()), GetPlayerPos(), materials, textures, biomesJson, blocksJson);
}

void VoxelNode::Process() {
	SM_PROFILE_ZONE;
	gameMode->Update(GetPlayerPos());
}

Camera3D *VoxelNode::GetCurrentCamera3D() const {
	Camera3D *camera = nullptr;
	if (Viewport* viewport = get_viewport())
	{
		camera = viewport->get_camera_3d();
	}
	return camera;
}

std::optional<std::string> VoxelNode::ReadFile(std::string_view file) const {
	SM_PROFILE_ZONE;
	String path = file.data();
	path.simplify_path();
	Error error;
	Ref<FileAccess> texturesFile = FileAccess::open(path, FileAccess::READ, &error);
	if (error)
		return {};
	return { std::string(texturesFile->get_as_text().utf8()) };
}

Vector3 VoxelNode::GetPlayerPos() const {
	Camera3D *camera = GetCurrentCamera3D();
	Vector3 playerPos = {};

	if (camera)
		playerPos = camera->get_global_position();
	return ConvertYZ(playerPos);
}

VoxelNode::VoxelNode() {
#ifdef TOOLS_ENABLED
	inEditor = Engine::get_singleton()->is_editor_hint();
#endif
	if(!inEditor)
		set_process(true);
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
