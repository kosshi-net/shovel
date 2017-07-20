// Standard
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Self
#include <renderer.hpp>

// Friends
#include <terrainmesher.hpp>

// Third party
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_STATIC
#include <GLFW\glfw3.h>

// Other
#include "../build/res.h"

#define PI 3.14159265

#define KeyEventStackSize 128

// kosshis renderer
namespace krdr {

	float randf(){
		return (float)rand()/RAND_MAX;
	}

	GLuint createShader(){
		srand(time(NULL));

		int InfoLogLength = 0;

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vertex_shader, NULL);
		glCompileShader(vs);
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fragment_shader, NULL);
		glCompileShader(fs);

		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			char * ProgramErrorMessage = (char*)malloc(InfoLogLength+1);
			glGetShaderInfoLog(vs, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("VERTEX SHADER COMPILATION FAILURE! LOG:\n%s\n", &ProgramErrorMessage[0]);
		}
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			char * ProgramErrorMessage = (char*)malloc(InfoLogLength+1);
			glGetShaderInfoLog(fs, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("FRAGMENT SHADER COMPILATION FAILURE! LOG:\n%s\n", &ProgramErrorMessage[0]);
		}

		GLuint shader_program = glCreateProgram();
		glAttachShader(shader_program, fs);
		glAttachShader(shader_program, vs);
		glLinkProgram(shader_program);

		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			char * ProgramErrorMessage = (char*)malloc(InfoLogLength+1);
			glGetProgramInfoLog(shader_program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("SHADERPROGRAM LINK FAILURE! LOG:\n%s\n", &ProgramErrorMessage[0]);
		}

		return shader_program;
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		printf("%i %i\n", width, height);
		glViewport(0, 0, width, height);
	}

	double scrollbuffer = 0;
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
		scrollbuffer += yoffset;
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


	GLFWwindow* window;

	GLuint shader;
	GLuint shader_uMP;
	GLuint shader_uMV;
	GLuint shader_aVertex;
	GLuint shader_aColor;

	int rendererInit(){
		if (!glfwInit())
			return -1;

		int width = 640, height = 480;
		window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);

		if (!window) {
			glfwTerminate();
			return -1;
		}

		glfwMakeContextCurrent(window);

		glewExperimental = GL_TRUE;
		GLenum game_glewinit = glewInit();

		if(game_glewinit){
			printf("Glew Failed: %s\n", glewGetErrorString(game_glewinit));
			return -1;
		}

		const GLubyte* renderer = glGetString(GL_RENDERER); 
		const GLubyte* version = glGetString(GL_VERSION); 
		printf("Renderer: %s\n", renderer);
		printf("OpenGL %s\n", version);

		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetKeyCallback(window, key_callback);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS); 


		glEnable(GL_CULL_FACE); 
		glCullFace(GL_BACK);
		
		// glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


		// SHADER
		shader = createShader();
		glUseProgram(shader);

		shader_uMP = glGetUniformLocation(shader, "uMP");
		shader_uMV = glGetUniformLocation(shader, "uMV");
		shader_aVertex = glGetAttribLocation(shader, "aVertex");
		shader_aColor = glGetAttribLocation(shader, "aColor");

		printf("Shader_aColor index: %u\n", shader_aColor );

		return 0;
	}


	void meshTick(){
		while(1){
			TerrainMesher::MeshBuffer* mesh = TerrainMesher::getMesh();
			if(mesh == NULL) break;

			int i = mesh->chunk;
			TerrainMesher::ChunkList* chunks = TerrainMesher::getChunks();

			// Check if buffers have ever been generated for this chunk, if no, gen!


			if( chunks->state[i] == 0 ){
				chunks->state[i]++;	

				glGenBuffers(1, &chunks->vertexBuffer[i] );
				glGenBuffers(1, &chunks->indexBuffer [i] );
				glGenBuffers(1, &chunks->colorBuffer [i] );

			}
			

			glBindBuffer(GL_ARRAY_BUFFER, chunks->vertexBuffer[i]);
			int size = mesh->vertexCount * sizeof(float);
			glBufferData(GL_ARRAY_BUFFER, size, mesh->vertexBuffer, GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, chunks->colorBuffer[i]);
			size = mesh->vertexCount * sizeof(float);
			glBufferData(GL_ARRAY_BUFFER, size, mesh->colorBuffer, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunks->indexBuffer[i]);
			size = mesh->indexCount * sizeof(int);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, mesh->indexBuffer, GL_STATIC_DRAW);

			chunks->items[i] = mesh->indexCount;


			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,	0);
			glBindBuffer(GL_ARRAY_BUFFER, 			0);

			TerrainMesher::meshUsed();
		}
	}

	void draw(glm::mat4 modelview, glm::mat4 projection){
		glUniformMatrix4fv(shader_uMV, 1, GL_FALSE, &modelview[0][0]);
		glUniformMatrix4fv(shader_uMP, 1, GL_FALSE, &projection[0][0]);

		meshTick();

		TerrainMesher::ChunkList* chunks = TerrainMesher::getChunks();

		glEnableVertexAttribArray(shader_aVertex);
		glEnableVertexAttribArray(shader_aColor);


		int count = 0;
		for (int i = 0; i < chunks->count; i++) {
			count++;
			if ( chunks->items[i] > 0 ) {

				glBindBuffer(GL_ARRAY_BUFFER, 			chunks->colorBuffer[i] );
				// glVertexAttribPointer(shader_aColor, 3, GL_BYTE, GL_TRUE, 0, NULL);
				glVertexAttribPointer(shader_aColor, 3, GL_FLOAT, GL_FALSE, 0, NULL);

				glBindBuffer(GL_ARRAY_BUFFER, 			chunks->vertexBuffer[i]);
				glVertexAttribPointer(shader_aVertex, 3, GL_FLOAT, GL_FALSE, 0, NULL);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,	chunks->indexBuffer [i]);

				glDrawElements(GL_TRIANGLES, chunks->items[i], GL_UNSIGNED_INT, 0);	
			}
		}
		// printf("%i\n", count);

	}









	double getTime(){
		return glfwGetTime();
	}

	int windowShouldClose(){
		return glfwWindowShouldClose(window);
	}

	void startFrame(){
		glClearColor(0.6, 0.6, 0.8, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void swapBuffers(){
		glfwSwapBuffers(window);
	}


	void getWindowSize(int *width, int *height){
		glfwGetWindowSize(window, width, height);
	}

	void setWindowTitle(char titlebuffer[]){
		glfwSetWindowTitle(window, titlebuffer);
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

} // end namespace