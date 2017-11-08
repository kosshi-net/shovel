#include <entity/entity.hpp>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <core/log.hpp>
#include <core/util.hpp>

#include <system/localinput.hpp>
#include <system/commands.hpp>

#include <terrain/terrain.hpp>

#include <physics/saabb.hpp>

using namespace ECS;
using namespace LocalInputSystem;

namespace PhysicsSystem {

	void run() {

		int count = entityCount();

		
		for (int i = 0; i < count; ++i) {
			Entity*e = getEntityByLocation(i);
			
			// if( 
			// 	hasComponents( e, 
			// 	   COMPONENT_VELOCITY 
			// 	 | COMPONENT_LOCATION 
			// 	) && 
			// 	!hasComponents( e, 
			// 	   COMPONENT_DEBUG_COLLISION
			// 	)
			// ){ 
			// 	for (int i = 0; i < 3; ++i) e->location[i] += e->velocity[i];
			// }

			if(  hasComponents( e, 
				  COMPONENT_VELOCITY 
				| COMPONENT_LOCATION
				| COMPONENT_DEBUG_COLLISION 
				| COMPONENT_DEBUG_COLLISION 
			)) {
				for (int i = 0; i < 3; ++i)
					e->last_location[i] = e->location[i];

				float time = 1;
				while(time > 0){
					float target[3];
					float area_min[3];
					float area_max[3];

					float e_min[3];
					float e_max[3];

					for (int i = 0; i < 3; ++i) {
						e_min[i] = e->location[i] + e->min[i];
						e_max[i] = e->location[i] + e->max[i];

						target[i] = e->location[i] + e->velocity[i];
						area_min[i] = std::min( e_min[i], target[i] );
						area_max[i] = std::max( e_max[i], target[i] );
					}

					int min[3];
					for (int i = 0; i < 3; ++i) 
						min[i] = (int)(area_min[i] + e->min[i]);

					int max[3];
					for (int i = 0; i < 3; ++i) 
						max[i] = (int)(area_max[i] + e->max[i] + 1);
					// convering to integer will floor it, +1 floor is ceiling

					float time_min = time;

					int il[3];
					float b_min[3];
					float b_max[3];
					float b_vel[3] = {0};
					int colnorm[3] = {0};
					for (il[0] = min[0]; il[0] < max[0]; il[0]++)
					for (il[1] = min[1]; il[1] < max[1]; il[1]++)
					for (il[2] = min[2]; il[2] < max[2]; il[2]++){
						if(Terrain::saferead(il)==0) continue;

						for (int i = 0; i < 3; ++i) {
							b_min[i] = il[i];
							b_max[i] = il[i]+1;
						}
						int _colnorm[3] = {0};
						// printf("%f %f %f\n", e_min[0], e_min[1], e_min[2]);
						// printf("%f %f %f\n", e_max[0], e_max[1], e_max[2]);
						// printf("%f %f %f\n", e->velocity[0], e->velocity[1], e->velocity[2]);
						// printf("%f %f %f\n", b_min[0], b_min[1], b_min[2]);
						// printf("%f %f %f\n", b_max[0], b_max[1], b_max[2]);

						float result = Physics::saabb(
							e_min,
							e_max,
							e->velocity,
							b_min,
							b_max,
							b_vel,
							_colnorm
						);
						
						if(result < time_min) {
							time_min = result;
							for(int i=0;i<3;++i) colnorm[i] = _colnorm[i];
						}
						
					}

					for (int i = 0; i < 3; ++i) {
						// e->location[i] += e->velocity[i]*(time_min);
						if( colnorm[i]!=0 ){
							float change = e->velocity[i]*(time_min-0.00001);

							// if(std::abs(change) > 0.00001) 
							e->velocity[i]+=change;

							e->velocity[i] = 0;
						}else{
							e->location[i] += e->velocity[i]*(time_min);
						}
					}

					time -= time_min;
				}
				e->velocity[1] -= 0.2;
				
			}


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

				if (hasCommand( e, CMD_MOVE_UP )) 
					e->velocity[1] += 1;

			}

			if( hasComponents( e, COMPONENT_VELOCITY | COMPONENT_AIRDRAG ) ) 
				for (int i = 0; i < 3; ++i) e->velocity[i] *= e->drag;


		}

	}

}