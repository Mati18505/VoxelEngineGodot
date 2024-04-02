#pragma once
#include <queue>
#include <functional>
#include <mutex>
#include <scene/resources/mesh.h>
#include <scene/resources/3d/world_3d.h>
#include <servers/rendering_server.h>

namespace Voxel {
	class ActorManager {
	public:
		static ActorManager& Get() {
			static ActorManager instance;
			return instance;
		}
		ActorManager(const ActorManager& other) = delete;
		ActorManager& operator=(const ActorManager& other) = delete;
	
		class Ref<World3D> world;
	
		void CreateActor(RID &out, const Transform3D &actorTransform);
		void UpdateMesh(RID actor, const Ref<Mesh> mesh);
		void DestroyActor(RID actor);
		
	private:
		ActorManager() = default;
	};
}
