#ifndef SHOVEL_GRAPHICS_CORE_INCLUDED
#define SHOVEL_GRAPHICS_CORE_INCLUDED 1


#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <mesher/terrainmesher.hpp>
#include <graphics/driver.h>


namespace graphics {
	int init(void);
	GLFWwindow* getWindow(void);

	GLuint createShader(const char vertSrc[], const char fragSrc[]);

	double getTime(void);

	int windowShouldClose(void);

	void startFrame(void);

	typedef struct {
		int chunksTotal;
		int chunksRendered;
		int drawCalls;
	} DebugInfo;
	DebugInfo* getDebugInfo();

	void swapBuffers(void);

	void getWindowSize(int *width, int *height);
	void setWindowTitle(char titlebuffer[]);

	void setClearColor(float, float, float, float);

	glm::vec3 screenToWorldSpaceVector(
		int x, int max_x, 
		int y, int max_y, 
		glm::mat4
	);

	const unsigned char* getRenderer();
	const unsigned char* getVersion() ;

	void toggleWireframe();

}

#endif