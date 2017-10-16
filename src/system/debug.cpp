#include <cstdio>
#include <cmath>
#include <entity/entity.hpp>
#include <system/localinput.hpp>
#include <core/log.hpp>

#include <system/commands.hpp>

#include <renderer/renderer.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <terrain/terrain.hpp>
#include <mesher/terrainmesher.hpp>
#include <core/util.hpp>

using namespace ECS;
using namespace LocalInputSystem;

/******************
Debug system
Temporary testing logic here
 ******************/

namespace DebugSystem {
	void run(){

		int count = entityCount();
		for (int i = 0; i < count; ++i) {
			Entity*e = getEntityByLocation(i);

			if( !hasComponents( e, 
				COMPONENT_DEBUG )
			) continue;

			// Logger::log("log");

			if(  hasCommand( e, CMD_ATTACK ) && 
				!hadCommand( e, CMD_ATTACK )
			){
				int width, height;
				Renderer::getWindowSize(&width, &height);
				float ratio = (float)width / (float)height;

				glm::mat4 projectionRotation = glm::perspective(
					90.0f, ratio, 0.01f, 1000.0f
				);

				projectionRotation = glm::rotate(
					projectionRotation, degToRad( e->pitch ),
					 glm::vec3(1.0f, 0.0f, 0.0f)
				);

				projectionRotation = glm::rotate(
					projectionRotation, degToRad( -e->yaw ), 
					glm::vec3(0.0f, 1.0f, 0.0f)
				);

				glm::vec3 vec = Renderer::screenToWorldSpaceVector(
					1, 2, 1, 2, projectionRotation);

				int hitloc[3]={0}; int normal[3]={0};
				char block = 0;  float dist = 2000.0;

				bool hit = Terrain::raycast(
					&vec[0], &e->location[0],
					&dist, hitloc, normal, &block
				);

				if(hit && Terrain::saferead(hitloc)){
					Terrain::write(hitloc, 0);
					TerrainMesher::markDirty(hitloc);
				}
				char txtbfr[128];
				sprintf(txtbfr, "Hit: %i, %i, %i", 
					hitloc[0],hitloc[1],hitloc[2]);
				Logger::log(txtbfr);

			}

			/*CODE HERE*/ 

		}
	}
}