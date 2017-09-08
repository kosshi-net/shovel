#include <cmath>
#include <other/util.hpp>

#include <entity/entity.hpp>
#include <input/input.hpp>
#include <input/keys.hpp>
using namespace ECS;

namespace LocalControlSystem {

	void run(bool cursorLocked){
		glfwPollEvents();
		double rx, ry;
		input::getCursorMovement(&rx, &ry);

		int count = entityCount();
		for (int i = 0; i < count; ++i) {
			Entity*e = getEntityByLocation(i);

			if( !hasComponents( e, 
				COMPONENT_VELOCITY | COMPONENT_LOCALCONTROL )
			) continue;


			if(cursorLocked){
				e->yaw+=rx*0.1f;
				e->pitch-=ry*0.1f;
			}

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

			if (input::getKey(KEY_S)) 
				for(int i = 0; i < 3; i++) e->velocity[i] += dir[i];

			if (input::getKey(KEY_W)) 
				for(int i = 0; i < 3; i++) e->velocity[i] -= dir[i];

			if (input::getKey(KEY_D)) 
				for(int i = 0; i < 3; i++) e->velocity[i] -= right[i];

			if (input::getKey(KEY_A)) 
				for(int i = 0; i < 3; i++) e->velocity[i] += right[i];



		}

	}


}