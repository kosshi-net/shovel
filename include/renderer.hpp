#ifndef KOSSHI_RENDERER_INCLUDED
#define KOSSHI_RENDERER_INCLUDED 1

#include <glm/mat4x4.hpp>
#include <terrainmesher.hpp>

namespace krdr {
	int rendererInit(void);

	double getTime(void);

	int windowShouldClose(void);

	void startFrame(void);

	void draw( glm::mat4, glm::mat4 );

	void swapBuffers(void);


	void getWindowSize(int *width, int *height);
	void setWindowTitle(char titlebuffer[]);

	int getKey(int key);
	int getMouseButton(int key);

	void getCursorPos(double *x, double *y);
	void getCursorMovement(double* x, double* y);
	void resetCursorMovement(void);

	int getScrollBuffer(void);
	void clearScrollBuffer(void);



	typedef struct {
		int key;
		int scancode;
		int action;
		int mods;
	} KeyEvent;


	int getKeyEventCount(void);

	void resetKeyEventCount(void);

	KeyEvent getKeyEvent(int index);


	void lockCursor();
	void unlockCursor();
}

#endif