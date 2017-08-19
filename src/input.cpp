#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_STATIC
#include <GLFW\glfw3.h>

#include <renderer.hpp>
#include <input.hpp>

#define MouseEventStackSize 128
#define KeyEventStackSize 128

namespace input {

	GLFWwindow * window;

	MouseEvent getMouseEvent(int index);

	MouseEvent MouseEventStack[MouseEventStackSize];
	int MouseEventCount = 0;

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
		if( MouseEventCount < MouseEventStackSize) {
			MouseEventStack[MouseEventCount].button = button;
			MouseEventStack[MouseEventCount].action = action;
			MouseEventStack[MouseEventCount].mods = mods;
			MouseEventCount++;
		} else {
			printf("Key event stack blown!\n");
		}
	}

	int getMouseEventCount(){
		return MouseEventCount;
	}

	void resetMouseEventCount(){
		MouseEventCount = 0;
	}

	MouseEvent getMouseEvent(int index){
		return MouseEventStack[index];
	}


	KeyEvent KeyEventStack[KeyEventStackSize];
	int KeyEventCount = 0;

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
		if(KeyEventCount < KeyEventStackSize) {
			KeyEventStack[KeyEventCount].key = key;
			KeyEventStack[KeyEventCount].scancode = scancode;
			KeyEventStack[KeyEventCount].action = action;
			KeyEventStack[KeyEventCount].mods = mods;
			KeyEventCount++;
		} else {
			printf("Key event stack blown!\n");
		}
	}

	int getKeyEventCount(){
		return KeyEventCount;
	}

	void resetKeyEventCount(){
		KeyEventCount = 0;
	}

	KeyEvent getKeyEvent(int index){
		return KeyEventStack[index];
	}


	double scrollbuffer = 0;
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
		scrollbuffer += yoffset;
	}


	int getKey(int key){
		return glfwGetKey(window, key);
	}

	int getMouseButton(int key){
		return glfwGetMouseButton(window, key);
	}

	void getCursorPos(double* x, double* y){
		glfwGetCursorPos(window, x, y);
	}

	double lastCursorPosX = 0;
	double lastCursorPosY = 0;

	void resetCursorMovement(){
		getCursorPos(&lastCursorPosX, &lastCursorPosY);
	}

	void getCursorMovement(double* x, double* y){
		double _x, _y;
		getCursorPos(&_x, &_y);
		*x = lastCursorPosX-_x;
		*y = lastCursorPosY-_y;
		lastCursorPosX = _x;
		lastCursorPosY = _y;
	}

	int getScrollBuffer(){
		return scrollbuffer;
	}
	void clearScrollBuffer(){
		scrollbuffer = 0;
	}


	void lockCursor(){
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	void unlockCursor(){
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}



	void init(GLFWwindow*w){

		window = w;

		glfwSetKeyCallback(window, key_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);

		
		glfwSetScrollCallback(window, scroll_callback);

	}

}