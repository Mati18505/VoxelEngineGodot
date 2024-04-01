#include "ActorManager.h"

namespace Voxel {
	void ActorManager::CreateActor(RID &out, const Ref<Mesh> mesh, const Transform3D &actorTransform) {
		RenderingServer* rs = RenderingServer::get_singleton();
		out = rs->instance_create2(mesh->get_rid(), world->get_scenario());
		rs->instance_set_transform(out, actorTransform);
	}
	
	void ActorManager::DestroyActor(RID actor)
	{
		RenderingServer::get_singleton()->free(actor);
	}
}
