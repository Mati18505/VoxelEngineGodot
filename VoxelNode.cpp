#include "VoxelNode.h"
#include "VoxelTypes.h"
#include <fstream>
#include "Tools/TextParser.h"
#include "Tools/ActorManager.h"

void VoxelNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_materials"), &VoxelNode::get_materials);
	ClassDB::bind_method(D_METHOD("set_materials", "materials"), &VoxelNode::set_materials);
	ClassDB::add_property("VoxelNode", PropertyInfo(Variant::DICTIONARY, "materials"), "set_materials", "get_materials");

	ClassDB::bind_method(D_METHOD("get_texture_array"), &VoxelNode::get_texture_array);
	ClassDB::bind_method(D_METHOD("set_texture_array", "textureArray"), &VoxelNode::set_texture_array);
	ClassDB::add_property("VoxelNode", PropertyInfo(Variant::OBJECT, "textureArray"), "set_texture_array", "get_texture_array");
}


void VoxelNode::LoadWorldConfig() {
	/*Voxel::TextParser textP;
	Ref<FileAccess> configFile = FileAccess::open("res://Assets/config.txt", FileAccess::READ);
	String configText = configFile->get_as_text();
	textP.Parse(std::string(configText.utf8()));*/
	
	Voxel::TextParser textP;
	std::string path = "D:/Projekty/Godot/VoxelEngine/VoxelWorld/Assets/config.txt";
	textP.Parse(path);

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

	Ref<FileAccess> biomesFile = FileAccess::open("res://Assets/biomes.json", FileAccess::READ);
	String biomesText = biomesFile->get_as_text();
	Voxel::Json biomesJson = Voxel::Json::parse(std::string(biomesText.utf8()));

	//FVector playerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	Vector3 playerLocation = get_position();
	world->Start(texturesJson, biomesJson, playerLocation);
}


void VoxelNode::Process() {
	//get_viewport()->get_camera_3d()->get_camera_transform();
	//Vector3 playerLocation = get_node("/root/player")->get_position();
	Vector3 playerLocation = get_position();
	world->Update(playerLocation);

	actorManagerQueue.Resolve();
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


void VoxelNode::set_texture_array(Ref<Texture2DArray> textureArray) {
	this->textureArray = textureArray;
}


Ref<Texture2DArray> VoxelNode::get_texture_array() const {
	return textureArray;
}
