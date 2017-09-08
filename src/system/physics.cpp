#include <entity/entity.hpp>
#include <log/log.hpp>


using namespace ECS;

namespace PhysicsSystem {

	void run() {

		int count = entityCount();
		for (int i = 0; i < count; ++i) {
			Entity*e = getEntityByLocation(i);
			
			if( hasComponents( e, COMPONENT_VELOCITY | COMPONENT_AIRDRAG ) ) 
				for (int i = 0; i < 3; ++i) e->velocity[i] *= e->drag;
			if( hasComponents( e, COMPONENT_VELOCITY | COMPONENT_LOCATION ) ) 
				for (int i = 0; i < 3; ++i) e->location[i] += e->velocity[i];


		}

	}

}