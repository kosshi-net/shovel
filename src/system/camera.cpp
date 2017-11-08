#include <entity/entity.hpp>

#include <graphics/core.hpp>
#include <graphics/voxel.hpp>
#include <graphics/box.hpp>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <core/util.hpp>
#include <core/log.hpp>

#include <ctime>

#define ENABLE_LERP true

using namespace ECS;

namespace CameraSystem {

	void draw( float lerp = 0 ){
		
		int width, height;
		int count = entityCount();
		Entity*c = NULL;
		// Search for entity with camera, break when found
		// Cameraentity will be "c"
		for (int i = 0; i < count; ++i){
			c = getEntityByLocation(i);
			if( hasComponents(c, 
				COMPONENT_CAMERA |
				COMPONENT_LOCATION
			)) break;
		}
		if(c==NULL) {
			Logger::error(
				"CAMERASYSTEM :: Unable to find entity with camera component");
			return;
		}

		graphics::startFrame();

		graphics::getWindowSize(&width, &height);

		// DEFINE CAMERA
		glm::mat4 projection;
		float ratio = (float)width /(float)height;
		projection = glm::perspective(90.0f, ratio, 0.01f, 350.0f);

		glm::mat4 projectionRotation = glm::perspective(
			90.0f, ratio, 0.01f, 1000.0f
		);

		projectionRotation = glm::rotate(
			projectionRotation, degToRad( c->pitch ),
			 glm::vec3(1.0f, 0.0f, 0.0f)
		);

		projectionRotation = glm::rotate(
			projectionRotation, degToRad( -c->yaw ), 
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		glm::mat4 view = glm::mat4();
		view = glm::rotate(
			view, degToRad(  c->pitch ), 
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		view = glm::rotate(
			view, degToRad( -c->yaw   ), 
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		glm::vec3 location = glm::make_vec3(c->location);

		glm::vec3 last_location = glm::make_vec3(c->last_location);
		glm::vec3 min = glm::make_vec3(c->min);

		if(ENABLE_LERP) location = glm::mix( last_location, location, lerp );

			// location + velocity * lerp;

		view = glm::translate	(view, -location);

		// DRAW TERRAIN
		graphics::voxel::draw(view, projection);

		// glm::mat4 mvp = projection*view;
		view = glm::translate	(view, location+min);



		view = glm::scale( view,  glm::vec3(0.8, 0.8, 0.8));

		graphics::box::draw_wireframe_cube(view, projection);

		// LOOP ENTITIES AND SEARCH FOR AABB GEOMETRY

	}

}