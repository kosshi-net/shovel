// Standard
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>

// Third party
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_STATIC
#include <GLFW\glfw3.h>

#include <other/koshmath.hpp>

#include <renderer/renderer.hpp>
#include <input/input.hpp>
#include <terrain/terrain.hpp>
#include <mesher/terrainmesher.hpp>

#include <log/log.hpp>

#include <other/defineterrain.hpp>


// Extension of self
#include <other/camera.hpp>


int main(void) {

	if(krdr::init() != 0){
		Logger::log("Could not init renderer\n");
		return 1;
	}
	int width, height;
	krdr::getWindowSize(&width, &height);


	float sx = 2.0 / width;
	float sy = 2.0 / height;

	krdr::setFogColor(0.7, 0.7, 0.9, 1.0);

	krdr::startFrame();
	krdr::drawText( "Please wait, loading... ", -1 + 8 * sx,   1 - 40 * sy,    sx, sy, 20 );
	krdr::drawText( "Copyright (c) kosshi.fi, do not distribute ", -1 + 8 * sx,   1 - 20 * sy,    sx, sy, 20 );
	krdr::swapBuffers();

	input::init(krdr::getWindow());

	glm::mat4 projection;

	Camera camera(glm::vec3(256.0f, 128.0f, 256.0f ), 180.0f, 0.0f);

	int TERRAIN_SIZE[] = {TERRAIN_SIZE_X, TERRAIN_SIZE_Y, TERRAIN_SIZE_Z};

	Logger::log("[Main] Initializing Terrain...\n");
	Terrain::init( TERRAIN_SIZE );

	Logger::log("[Main] Initializing TerrainMesher...\n");
	TerrainMesher::init(TERRAIN_SIZE, CHUNK_ROOT);

	

	double lasttime = krdr::getTime();
	// double lasttick = getTime();
 
	bool cursorLocked = false;
	bool drawText = true;


	input::resetCursorMovement();

	char title[] = "ShovelEngine";
	char build[] = "DEV-0.0.16";
	krdr::setWindowTitle(title);

	krdr::setFogColor(0.7, 0.7, 0.9, 1.0);


	Logger::log("[Main] Startup done!\n");


	while(!krdr::windowShouldClose()) {
		double currenttime = krdr::getTime();
		double delta = currenttime-lasttime;

		krdr::startFrame();

		krdr::getWindowSize(&width, &height);

		double cx, cy;
		input::getCursorPos(&cx, &cy);
		if(cursorLocked){
			cx = width*0.5;
			cy = height*0.5;
		}

		float ratio = (float)width /(float)height;
		projection = glm::perspective(90.0f, ratio, 0.01f, 1000.0f);

		glm::mat4 projectionRotation = glm::perspective(90.0f, ratio, 0.01f, 1000.0f);
		projectionRotation = glm::rotate(projectionRotation, degToRad(camera.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		projectionRotation = glm::rotate(projectionRotation, degToRad(-camera.yaw),  glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 view = glm::mat4();
		view = glm::rotate		(view, degToRad(camera.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate		(view, degToRad(-camera.yaw),  glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::translate	(view, -glm::vec3(camera.loc[0], camera.loc[1], camera.loc[2]));

		// glm::mat4 model = glm::mat4();

		krdr::draw(view, projection);


		
			
		if(drawText){ 
			sx = 2.0 / width;
			sy = 2.0 / height;


			// glm::vec3 vec = krdr::screenToWorldSpaceVector(cx, width, cy, height, projectionRotation);

			int line = 1;

			static char txtbfr[256];

			snprintf(txtbfr, sizeof(txtbfr), "%ifps", (int)(1.0/delta));
			krdr::drawText( txtbfr, -1 + 8 * sx,   1 - (20*line++) * sy,    sx, sy, 20 );
			snprintf(txtbfr, sizeof(txtbfr), "[%i, %i, %i]", (int)camera.loc[0], (int)camera.loc[1], (int)camera.loc[2]);
			krdr::drawText( txtbfr, -1 + 8 * sx,   1 - (20*line++) * sy,    sx, sy, 20 );

			krdr::drawText( (char*)krdr::getRenderer(), -1 + 8 * sx,   1 - (20*line++) * sy,    sx, sy, 20 );
			krdr::drawText( (char*)krdr::getVersion(), -1 + 8 * sx,   1 - (20*line++) * sy,    sx, sy, 20 );

			krdr::drawText( build, -1 + 8 * sx,   1 - (20*line++) * sy,    sx, sy, 20 );
			krdr::drawText( "Do not distribute", -1 + 8 * sx,   1 - (20*line++) * sy,    sx, sy, 20 );

			snprintf(txtbfr, sizeof(txtbfr), "Mshr: %i%%, %i", (int)(TerrainMesher::getActivity()*100), TerrainMesher::getCount());
			krdr::drawText( txtbfr, -1 + 8 * sx,   1 - (20*line++) * sy,    sx, sy, 20 );

			


			// snprintf(titlebuffer, sizeof(titlebuffer), "[%f, %f, %f]", vec[0], vec[1], vec[2]);
			// krdr::drawText( titlebuffer, -1 + 8 * sx,   1 - (20*line++) * sy,    sx, sy, 20 );



			// int hitloc[3]={0};
			// int normal[3]={0};
			// char block = 0;
			// float dist = 2000.0;
			// Terrain::raycast(
				// &vec[0], &camera.loc[0],
				// &dist, hitloc, normal, &block
			// );
			// snprintf(titlebuffer, sizeof(titlebuffer), "hit: %f", dist);
			// krdr::drawText( titlebuffer, -1 + 8 * sx,   1 - (20*line++) * sy,    sx, sy, 20 );


		}

		glfwPollEvents();

		double rx, ry;
		input::getCursorMovement(&rx, &ry);


		if(cursorLocked){
			camera.yaw+=rx*0.1f;
			camera.pitch-=ry*0.1f;
		}

		if(input::getMouseButton(1)){
			

			glm::vec3 vec = krdr::screenToWorldSpaceVector(cx, width, cy, height, projectionRotation);

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


		float pitch = degToRad(-camera.pitch);
		float yaw = degToRad(camera.yaw);

		float s = 20.0f;


		glm::vec3 dir(
			cosf(pitch) * sinf(yaw),
			sinf(-pitch),
			cosf(pitch) * cosf(yaw)
		);
		glm::vec3 right(
			sinf(yaw - 3.14/2), 
			0,
			cosf(yaw - 3.14/2)
		);

		right*=s;
		dir*=s;

		right*=delta*5;
		dir*=delta*5;

		if (input::getKey(GLFW_KEY_S)) camera.loc+=dir;
		if (input::getKey(GLFW_KEY_W)) camera.loc-=dir;
		if (input::getKey(GLFW_KEY_D)) camera.loc-=right;
		if (input::getKey(GLFW_KEY_A)) camera.loc+=right;


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
						krdr::toggleWireframe();
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
			if(e.action) {
				switch (e.button){
					case 0:

						if(!input::getKey(GLFW_KEY_LEFT_SHIFT) && !cursorLocked){
							input::lockCursor();
							cursorLocked = true;
						} else {

							glm::vec3 vec = krdr::screenToWorldSpaceVector(cx, width, cy, height, projectionRotation);

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
				}
				
			}
		}
		input::resetMouseEventCount();

		krdr::swapBuffers();

		lasttime = currenttime;
	}

	glfwTerminate();
	return 0;
}

