#ifndef KOSSHI_RENDERER_INCLUDED
#define KOSSHI_RENDERER_INCLUDED 1

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <terrainmesher.hpp>

#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_STATIC
#include <GLFW\glfw3.h>

namespace krdr {
	int init(void);
	GLFWwindow* getWindow(void);

	double getTime(void);

	int windowShouldClose(void);

	void startFrame(void);

	void draw( glm::mat4, glm::mat4 );

	void swapBuffers(void);


	void getWindowSize(int *width, int *height);
	void setWindowTitle(char titlebuffer[]);


	void drawText(const char*, float, float, float, float, int);

	void setFogColor(float, float, float, float);


	glm::vec3 screenToWorldSpaceVector(int x, int max_x, int y, int max_y, glm::mat4);



	void toggleWireframe();

}

#endif