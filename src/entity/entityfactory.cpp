#include <cstdio>
#include <entity/entity.hpp>
#include <core/log.hpp>
using namespace ECS;

namespace EntityFactory {

	bool createPlayer(  ){
		
		Logger::log("ENTITIYFACTORY :: Creating player...");

		int count = entityCount();
		for (int i = 0; i < count; ++i) {

			Entity*e = getEntityByLocation( i );

			if( hasComponents( e, COMPONENT_NONE ) ) {

				setComponents( e,
					  COMPONENT_LOCATION
					| COMPONENT_VELOCITY
					| COMPONENT_AIRDRAG
					| COMPONENT_CAMERA
					| COMPONENT_INPUT_LOCAL
					| COMPONENT_CONTROL_MODEL_BASIC
					| COMPONENT_DEBUG
				);

				e->speed = 0.0f;
				e->acceleration = 0.2f;
				e->drag = 0.9f;
				e->location[0] = 32;
				e->location[1] = 128;
				e->location[2] = 32;


				e->velocity[0] = 0;
				e->velocity[1] = 0;
				e->velocity[2] = 0;


				e->yaw = 0;
				e->pitch = 0;

				return true;
			}
		}
		Logger::error("ENTITIYFACTORY :: ERROR :: OUT OF ENTITIES!?");
		return false;
	}


}