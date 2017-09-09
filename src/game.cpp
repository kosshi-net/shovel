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

#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_STATIC
#include <GLFW\glfw3.h>

#include <renderer/renderer.hpp>
#include <input/input.hpp>
#include <terrain/terrain.hpp>
#include <mesher/terrainmesher.hpp>
#include <log/log.hpp>

#include <event/event.hpp>

#include <entity/entity.hpp>
#include <entity/entityfactory.hpp>

#include <system/localcontrol.hpp>
#include <system/physics.hpp>
#include <system/camera.hpp>

#include <other/defineterrain.hpp>


#define SHOVEL_VERSION "DEV-0.0.19"

#define TICK CLOCKS_PER_SEC/60
#define SKIP TICK*10

int main(void) {

	Logger::init();



	if(Renderer::init() != 0){
		Logger::error("MAIN :: ERROR:: Could not init renderer\n");
		return 1;
	}
	int width, height;
	Renderer::getWindowSize(&width, &height);


	float sx = 2.0 / width;
	float sy = 2.0 / height;

	Renderer::setFogColor(0.7, 0.7, 0.9, 1.0);

	Renderer::startFrame();
	Renderer::drawText( "Please wait, loading... ", 
		-1 + 8 * sx,   1 - 40 * sy, sx, sy, 20 );
	Renderer::drawText( "Copyright (c) kosshi.fi, do not distribute ", 
		-1 + 8 * sx,   1 - 20 * sy, sx, sy, 20 );
	Renderer::swapBuffers();

	input::init(Renderer::getWindow());
	ECS::init();

	EntityFactory::createPlayer();

	int TERRAIN_SIZE[] = {TERRAIN_SIZE_X, TERRAIN_SIZE_Y, TERRAIN_SIZE_Z};

	Logger::log("MAIN :: Initializing Terrain...");
	Terrain::init( TERRAIN_SIZE );

	Logger::log("MAIN :: Initializing TerrainMesher...");
	TerrainMesher::init(TERRAIN_SIZE, CHUNK_ROOT);

	

	double lasttime = Renderer::getTime();
	// double lasttick = getTime();
 
	bool cursorLocked = false;
	bool drawText = true;


	input::resetCursorMovement();

	char title[] = "ShovelEngine";
	char build[] = SHOVEL_VERSION;
	Renderer::setWindowTitle(title);

	Renderer::setFogColor(0.7, 0.7, 0.9, 1.0);


	Logger::log("MAIN :: Startup done!");

	clock_t physTime = 0;

	static char txtbfr[256];

	while(!Renderer::windowShouldClose()) {
		double currenttime = Renderer::getTime();
		double delta = currenttime-lasttime;


		Renderer::getWindowSize(&width, &height);

		clock_t now = clock();
		CameraSystem::draw( (now - physTime) / (float)TICK );

		// DEBUG
		if(drawText){ 
			sx = 2.0 / width;
			sy = 2.0 / height;


			// glm::vec3 vec = Renderer::screenToWorldSpaceVector(
			// 		cx, width, cy, height, projectionRotation);

			int line = 1;


			snprintf(txtbfr, sizeof(txtbfr), "%ifps", (int)(1.0/delta));
			Renderer::drawText( txtbfr,
				-1 + 8 * sx,   1 - (20*line++) * sy,    sx, sy, 20 );

			Renderer::drawText( (char*)Renderer::getRenderer(),
				-1 + 8 * sx, 1 - (20*line++) * sy, sx, sy, 20 );

			Renderer::drawText( (char*)Renderer::getVersion(),
				-1 + 8 * sx, 1 - (20*line++) * sy, sx, sy, 20 );

			Renderer::drawText( build,
				-1 + 8 * sx, 1 - (20*line++) * sy, sx, sy, 20 );

			Renderer::drawText( "Do not distribute",
				-1 + 8 * sx, 1 - (20*line++) * sy, sx, sy, 20 );

			snprintf(txtbfr, sizeof(txtbfr), "Mshr: %i%%, %i", 
				(int)(TerrainMesher::getActivity()*100), 
				TerrainMesher::getCount()
			);
			Renderer::drawText( txtbfr, 
				-1 + 8 * sx, 1 - (20*line++) * sy, sx, sy, 20 );

			snprintf(txtbfr, sizeof(txtbfr), 
				"Lerp: %f", 
				(now - physTime) / (float)TICK
			);

			Renderer::drawText( txtbfr, 
				-1 + 8 * sx, 1 - (20*line++) * sy, sx, sy, 20 );
		}


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
			LocalControlSystem::run(cursorLocked);
			PhysicsSystem::run();
		}

		double cx, cy;
		input::getCursorPos(&cx, &cy);
		if(cursorLocked){
			cx = width*0.5;
			cy = height*0.5;
		}
		/*
		if(input::getMouseButton(1)){
			

			glm::vec3 vec = Renderer::screenToWorldSpaceVector(
				cx, width, cy, height, projectionRotation);

			int hitloc[3]={0};
			int normal[3]={0};
			char block = 0;
			float dist = 2000.0;

			bool hit = Terrain::raycast(
				&vec[0], &camera.loc[0],
				&dist, hitloc, normal, &block
			);

			if(hit) {
				// if(Terrain::saferead(hitloc)){
					// Terrain::write(hitloc, 0);
					// TerrainMesher::markDirty(hitloc);
				// }
				for (int x = -3; x < 3; ++x)
				for (int y = -3; y < 3; ++y)
				for (int z = -3; z < 3; ++z)
				{	
					int ll[] ={
						hitloc[0]+x,
						hitloc[1]+y,
						hitloc[2]+z
					};
					if(Terrain::saferead(ll)){
						Terrain::write(ll, 0);
						TerrainMesher::markDirty(ll);
					}
				}

			}
		}
		*/


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
						Renderer::toggleWireframe();
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
					/*
					else {

						glm::vec3 vec = 
						Renderer::screenToWorldSpaceVector(
							cx, width, cy, height, projectionRotation);

						int hitloc[3]={0}; int normal[3]={0};
						char block = 0;  float dist = 2000.0;

						bool hit = Terrain::raycast(
							&vec[0], &camera.loc[0],
							&dist, hitloc, normal, &block
						);

						if(hit && Terrain::saferead(hitloc)){
							Terrain::write(hitloc, 0);
							TerrainMesher::markDirty(hitloc);
						}
						break;
					}
						*/

			}
			
		
		}
		input::resetMouseEventCount();

		Renderer::swapBuffers();

		lasttime = currenttime;
	}
	Logger::log("MAIN :: Shutting down..");
	EventSystem::fire( EventSystem::EVENT_SHUTDOWN, NULL );
	glfwTerminate();
	return 0;
}

