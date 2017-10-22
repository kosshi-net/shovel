#include <entity/entity.hpp>
#include <renderer/renderer.hpp>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <core/util.hpp>

#include <ctime>

#define ENABLE_LERP true

using namespace ECS;

namespace CameraSystem {

	void draw( float lerp = 0 ){
		
		int width, height;
		int count = entityCount();
		Entity*e = NULL;
		for (int i = 0; i < count; ++i){
			e = getEntityByLocation(i);
			if( hasComponents(e, 
					  COMPONENT_CAMERA
					| COMPONENT_LOCATION
				) 
			) break;
		}
		if(e==NULL) return;

		Renderer::startFrame();

		Renderer::getWindowSize(&width, &height);

		// DEFINE CAMERA
		glm::mat4 projection;
		float ratio = (float)width /(float)height;
		projection = glm::perspective(90.0f, ratio, 0.01f, 350.0f);

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

		glm::mat4 view = glm::mat4();
		view = glm::rotate(
			view, degToRad(  e->pitch ), 
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		view = glm::rotate(
			view, degToRad( -e->yaw   ), 
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		glm::vec3 location = glm::vec3(
			e->location[0], e->location[1], e->location[2]
		);
		glm::vec3 last_location = glm::vec3(
			e->last_location[0], e->last_location[1], e->last_location[2]
		);

		if(ENABLE_LERP) location = glm::mix( last_location, location, lerp );

			// location + velocity * lerp;

		view = glm::translate	(view, -location);

		// DRAW
		Renderer::draw(view, projection);

	}

}