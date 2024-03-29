// Standard
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>
#include <limits.h>

// Third party
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <graphics/driver.h>
#include <graphics/core.hpp>
#include <graphics/voxel.hpp>
#include <graphics/text.hpp>

#include <input/input.hpp>
#include <terrain/terrain.hpp>
#include <mesher/terrainmesher.hpp>
#include <core/log.hpp>

#include <event/event.hpp>

#include <entity/entity.hpp>
#include <entity/entityfactory.hpp>

#include <system/localinput.hpp>
#include <system/physics.hpp>
#include <system/camera.hpp>
#include <system/debug.hpp>

#include <core/defineterrain.hpp>


#define SHOVEL_VERSION "d0.0.27"

#define TICK (CLOCKS_PER_SEC/30)
#define SKIP TICK*10

int main(void) {

	Logger::init();



	if(graphics::init() != 0){
		Logger::error("MAIN :: ERROR:: Could not init renderer\n");
		return 1;
	}
	int width, height;
	graphics::getWindowSize(&width, &height);


	float sx = 2.0 / width;
	float sy = 2.0 / height;

	graphics::voxel::setFogColor(1.0, 1.0, 1.0, 1.0);

	graphics::startFrame();
	graphics::text::draw( "Please wait, generating terrain... ", 
		-1 + 8 * sx,   1 - 40 * sy, sx, sy, 20 );
	graphics::text::draw( "Copyright (c) kosshi.fi, do not reupload and stuff, please", 
		-1 + 8 * sx,   1 - 20 * sy, sx, sy, 20 );
	graphics::swapBuffers();

	input::init(graphics::getWindow());
	ECS::init();

	EntityFactory::createPlayer();
	EntityFactory::createDebugCube();

	int TERRAIN_SIZE[] = {TERRAIN_SIZE_X, TERRAIN_SIZE_Y, TERRAIN_SIZE_Z};

	Terrain::init( TERRAIN_SIZE );
	TerrainMesher::init(TERRAIN_SIZE, CHUNK_ROOT);

	

	double lasttime = graphics::getTime();
	// double lasttick = getTime();
 
	bool cursorLocked = false;
	bool drawText = true;


	input::resetCursorMovement();

	char title[] = "ShovelEngine";
	char build[] = SHOVEL_VERSION;
	graphics::setWindowTitle(title);

	graphics::voxel::setFogColor(0.7, 0.7, 0.9, 1.0);


	Logger::log("MAIN :: Startup done!");

	clock_t physTime = 0;

	static char txtbfr[256];

	while(!graphics::windowShouldClose()) {
		double currenttime = graphics::getTime();
		double delta = currenttime-lasttime;


		graphics::getWindowSize(&width, &height);

		clock_t now = clock();
		float lerp = (float)(now - physTime) / (float)TICK;
		CameraSystem::draw( lerp );

		// DEBUG
		if(drawText){ 
			sx = 2.0 / width;
			sy = 2.0 / height;


			// glm::vec3 vec = Renderer::screenToWorldSpaceVector(
			// 		cx, width, cy, height, projectionRotation);

			int line = 1;


			snprintf(txtbfr, sizeof(txtbfr), "%ifps", (int)(1.0/delta));
			graphics::text::draw( txtbfr,
				-1 + 8 * sx,   1 - (20*line++) * sy,    sx, sy, 20 );

			graphics::text::draw( (char*)graphics::getRenderer(),
				-1 + 8 * sx, 1 - (20*line++) * sy, sx, sy, 20 );

			graphics::text::draw( (char*)graphics::getVersion(),
				-1 + 8 * sx, 1 - (20*line++) * sy, sx, sy, 20 );

			graphics::text::draw( build,
				-1 + 8 * sx, 1 - (20*line++) * sy, sx, sy, 20 );

			snprintf(txtbfr, sizeof(txtbfr), "Mshr: %i%%, %i", 
				(int)(TerrainMesher::getActivity()*100), 
				TerrainMesher::getCount()
			);
			graphics::text::draw( txtbfr, 
				-1 + 8 * sx, 1 - (20*line++) * sy, sx, sy, 20 );

			snprintf(txtbfr, sizeof(txtbfr), "Lerp: %f, %ihz", 
				lerp, CLOCKS_PER_SEC/TICK );
			graphics::text::draw( txtbfr, 
				-1 + 8 * sx, 1 - (20*line++) * sy, sx, sy, 20 );

			snprintf(txtbfr, sizeof(txtbfr), "Draw calls: %i", 
				graphics::getDebugInfo()->drawCalls );
			graphics::text::draw( txtbfr, 
				-1 + 8 * sx, 1 - (20*line++) * sy, sx, sy, 20 );

			ECS::Entity *e = ECS::getEntityByLocation(0);

			snprintf(txtbfr, sizeof(txtbfr), "min: %f %f %f max: %f %f %f", 
				e->location[0]+e->min[0],
				e->location[1]+e->min[1],
				e->location[2]+e->min[2],
				e->location[0]+e->max[0],
				e->location[1]+e->max[1],
				e->location[2]+e->max[2]
			 );
			graphics::text::draw( txtbfr, 
				-1 + 8 * sx, 1 - (20*line++) * sy, sx, sy, 20 );
		}

		LocalInputSystem::mouse(cursorLocked);
		
		if( now - physTime > TICK ) {
			if( now - physTime > SKIP ){
				snprintf(txtbfr, sizeof(txtbfr), 
					"MAIN :: WARN :: Can't keep up! Skipped %li!", 
					(now - physTime) 
				);
				Logger::warn( txtbfr );
				physTime = now;
			} else {
				physTime += TICK;
			}
			if(cursorLocked) LocalInputSystem::tick();
			PhysicsSystem::run();
			DebugSystem::run();
		}

		double cx, cy;
		input::getCursorPos(&cx, &cy);
		if(cursorLocked){
			cx = width*0.5;
			cy = height*0.5;
		}

		static int kec = 0;
		kec = input::getKeyEventCount();
		for (int i = 0; i < kec; ++i) {
			input::KeyEvent e = input::getKeyEvent(i);
			// printf("Keycode: %i, %i\n", e.key, e.action);
			if(e.action) {
				switch (e.key){
					case GLFW_KEY_ESCAPE:
						input::unlockCursor();
						cursorLocked = false;
						break;
					case GLFW_KEY_F1:
						drawText = !drawText;
						break;
					case GLFW_KEY_F2:
						graphics::toggleWireframe();
						break;

				}
				
			}
		}
		input::resetKeyEventCount();

		kec = 0;
		kec = input::getMouseEventCount();
		for (int i = 0; i < kec; ++i) {
			input::MouseEvent e = input::getMouseEvent(i);
			// printf("Keycode: %i, %i\n", e.key, e.action);
			if(e.action)
			switch (e.button){
				case 0:

					if( !input::getKey(GLFW_KEY_LEFT_SHIFT) 
						&& !cursorLocked
					){
						input::lockCursor();
						cursorLocked = true;
					} 
			}
		}
		input::resetMouseEventCount();

		graphics::swapBuffers();

		lasttime = currenttime;
	}
	Logger::log("MAIN :: Shutting down..");
	EventSystem::fire( EventSystem::EVENT_SHUTDOWN, NULL );
	glfwTerminate();
	Logger::log("MAIN :: Goodbye!");
	return 0;
}

