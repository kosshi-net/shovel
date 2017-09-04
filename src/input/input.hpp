#ifndef KOSSHI_INPUT_INCLUDED
#define KOSSHI_INPUT_INCLUDED 1

namespace input {
	void init(GLFWwindow*);

	typedef struct {
		int button;
		int action;
		int mods;
	} MouseEvent;

	MouseEvent getMouseEvent(int index);

	int    getMouseEventCount(void);
	void resetMouseEventCount(void);



	typedef struct {
		int key;
		int scancode;
		int action;
		int mods;
	} KeyEvent;

	KeyEvent getKeyEvent(int index);

	int    getKeyEventCount(void);
	void resetKeyEventCount(void);

	void lockCursor();
	void unlockCursor();

	int getKey(int key);
	int getMouseButton(int key);

	void getCursorPos(double *x, double *y);
	void getCursorMovement(double* x, double* y);
	void resetCursorMovement(void);

	int getScrollBuffer(void);
	void clearScrollBuffer(void);

}

#endif