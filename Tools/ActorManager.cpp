#include "ActorManager.h"
#include "Profiler.h"

namespace Voxel {
	void ActorManager::CreateActor(RID &out, const Transform3D &actorTransform) {
		SM_PROFILE_ZONE;
		RenderingServer *rs = RenderingServer::get_singleton();
		out = rs->instance_create();
		rs->instance_set_scenario(out, world->get_scenario());
		rs->instance_set_transform(out, actorTransform);
	}

	void ActorManager::UpdateMesh(RID actor, const Ref<Mesh> mesh) {
		SM_PROFILE_ZONE;
		RenderingServer::get_singleton()->instance_set_base(actor, mesh->get_rid());
	}
	
	void ActorManager::DestroyActor(RID actor)
	{
		SM_PROFILE_ZONE;
		RenderingServer::get_singleton()->free(actor);
	}
}
