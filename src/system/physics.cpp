#include <entity/entity.hpp>
#include <cmath>
#include <core/log.hpp>
#include <core/util.hpp>

#include <system/localinput.hpp>
#include <system/commands.hpp>

using namespace ECS;
using namespace LocalInputSystem;

namespace PhysicsSystem {

	void run() {

		int count = entityCount();

		
		for (int i = 0; i < count; ++i) {
			Entity*e = getEntityByLocation(i);
				
			if( hasComponents( e, COMPONENT_VELOCITY | COMPONENT_LOCATION ) ) 
				for (int i = 0; i < 3; ++i) e->location[i] += e->velocity[i];

			if( hasComponents( e, 
				  COMPONENT_VELOCITY 
				| COMPONENT_CONTROL_MODEL_BASIC 
				) 
			){


				float pitch = degToRad(-e->pitch);
				float yaw = degToRad(e->yaw);

				float s = e->acceleration;

				float dir[] = {
					cosf(pitch) * sinf(yaw)  * s,
					sinf(-pitch)		 	 * s,
					cosf(pitch) * cosf(yaw)  * s
				};

				float right[] = {
					sinf(yaw - 3.14/2) * s, 
					0,
					cosf(yaw - 3.14/2) * s
				};

				if (hasCommand( e, CMD_MOVE_BACKWARD )) 
					for(int i = 0; i < 3; i++) e->velocity[i] += dir[i];

				if (hasCommand( e, CMD_MOVE_FORWARD )) 
					for(int i = 0; i < 3; i++) e->velocity[i] -= dir[i];

				if (hasCommand( e, CMD_MOVE_RIGHT )) 
					for(int i = 0; i < 3; i++) e->velocity[i] -= right[i];

				if (hasCommand( e, CMD_MOVE_LEFT )) 
					for(int i = 0; i < 3; i++) e->velocity[i] += right[i];

			}

			if( hasComponents( e, COMPONENT_VELOCITY | COMPONENT_AIRDRAG ) ) 
				for (int i = 0; i < 3; ++i) e->velocity[i] *= e->drag;


		}

	}

}