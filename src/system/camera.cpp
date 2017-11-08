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

	glm::vec3 getLerpLocation(Entity*e, float lerp) {
		return glm::mix( 
			glm::make_vec3(e->last_location),
			glm::make_vec3(e->location), 
			lerp 
		);
	}

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

		glm::vec3 location = getLerpLocation( c, lerp );


		view = glm::translate	(view, -location);

		// DRAW TERRAIN
		graphics::voxel::draw(view, projection);


		// view = glm::translate	(view, location);
		// view = glm::scale( view,  glm::vec3(0.8, 0.8, 0.8));

		// LOOP ENTITIES AND SEARCH FOR AABB GEOMETRY
		Entity*e = NULL;
		for (int i = 0; i < count; ++i){
			e = getEntityByLocation(i);

			glm::mat4 view2 = view;

			if( !hasComponents(e, COMPONENT_AABB_GEOMETRY) ) continue;

			glm::vec3 loc = getLerpLocation( e, lerp );
			glm::vec3 min = glm::make_vec3(e->min);
			glm::vec3 max = glm::make_vec3(e->max);
			
			view2 = glm::translate	(view2, loc+min);
			view2 = glm::scale( view2, -min+max );

			graphics::box::draw_wireframe_cube(view2, projection);

		}

	}

}