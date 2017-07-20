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

#include <GLFW\glfw3.h>

#include <koshmath.hpp>

#include <renderer.hpp>
#include <terrain.hpp>
#include <terrainmesher.hpp>

#include <defineterrain.hpp>


// Extension of self
#include <camera.hpp>

int main(void) {

	if(krdr::rendererInit() != 0){
		printf("ERROR\n");
		return -1;
	}

	glm::mat4 projection;

	Camera camera(glm::vec3(-128.0f, -128.0f, -128.0f ), 180.0f, 0.0f);

	int TERRAIN_SIZE[] = {TERRAIN_SIZE_X, TERRAIN_SIZE_Y, TERRAIN_SIZE_Z};

	printf("Initializing Terrain...\n");
	Terrain::init( TERRAIN_SIZE );

	printf("Initializing TerrainMesher...\n");
	TerrainMesher::init(TERRAIN_SIZE, CHUNK_ROOT);

	printf("START GAMELOOP\n");

	double lasttime = krdr::getTime();
	// double lasttick = getTime();
 
	bool cursorLocked = false;

	krdr::resetCursorMovement();
	while(!krdr::windowShouldClose()) {
		double currenttime = krdr::getTime();
		double delta = currenttime-lasttime;

		krdr::startFrame();

		int width, height;
		krdr::getWindowSize(&width, &height);

		float ratio = (float)width /(float)height;
		projection = glm::perspective(90.0f, ratio, 0.01f, 1000.0f);

		glm::mat4 view = glm::mat4();
		view = glm::rotate(view, degToRad(camera.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, degToRad(-camera.yaw),  glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::translate(view, glm::vec3(camera.loc[0], camera.loc[1], camera.loc[2]));

		// glm::mat4 model = glm::mat4();

		krdr::draw(view, projection);
		
		static char titlebuffer[256];
		snprintf(titlebuffer, sizeof(titlebuffer), "OpenGL [%ifps]", (int)(1.0/delta));
		krdr::setWindowTitle(titlebuffer);
		


		glfwPollEvents();

		double rx, ry;
		krdr::getCursorMovement(&rx, &ry);

		// if ( krdr::getMouseButton(0) ){
		// 	krdr::lockCursor();

		// 	camera.yaw+=rx*0.1f;
		// 	camera.pitch-=ry*0.1f;
		// } else {
		// 	krdr::unlockCursor();
		// }

		if(cursorLocked){
			camera.yaw+=rx*0.1f;
			camera.pitch-=ry*0.1f;
		}

		float pitch = degToRad(-camera.pitch);
		float yaw = degToRad(camera.yaw);

		float s = 5.0f;


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

		if (krdr::getKey(GLFW_KEY_S)) camera.loc-=dir;
		if (krdr::getKey(GLFW_KEY_W)) camera.loc+=dir;
		if (krdr::getKey(GLFW_KEY_D)) camera.loc+=right;
		if (krdr::getKey(GLFW_KEY_A)) camera.loc-=right;


		static int kec = 0;
		kec = krdr::getKeyEventCount();
		for (int i = 0; i < kec; ++i) {
			krdr::KeyEvent e = krdr::getKeyEvent(i);
			// printf("Keycode: %i, %i\n", e.key, e.action);
			if(e.action) {
				switch (e.key){
					case GLFW_KEY_F2:
						krdr::unlockCursor();
						cursorLocked = false;
						break;
					case GLFW_KEY_F1:
						krdr::lockCursor();
						cursorLocked = true;
						break;

				}
				
			}
		}
		krdr::resetKeyEventCount();

		krdr::swapBuffers();

		lasttime = currenttime;
	}

	glfwTerminate();
	return 0;
}

