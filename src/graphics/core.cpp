// Standard
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <mesher/terrainmesher.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <core/log.hpp>


#include <windows.h>

#define PI 3.14159265

#define MSAA_SAMPLES 2

#include <graphics/driver.h>
#include <graphics/cull.hpp>
#include <graphics/text.hpp>
#include <graphics/voxel.hpp>

namespace graphics {

	typedef struct {
		int chunksTotal;
		int chunksRendered;
		int drawCalls;
	} DebugInfo;
	DebugInfo debugInfo;
	DebugInfo* getDebugInfo(){
		return &debugInfo;
	}

	GLuint createShader(const char vertSrc[], const char fragSrc[]){

		int InfoLogLength = 0;

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vertSrc, NULL);
		glCompileShader(vs);
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fragSrc, NULL);
		glCompileShader(fs);

		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			char * err = (char*)malloc(InfoLogLength+1);
			glGetShaderInfoLog(vs, InfoLogLength, NULL, 
				&err[0]);
			printf("Vertex shader failed to compile! LOG:\n%s\n", 
				&err[0]);
			MessageBox(0, "Vertex shader failed to compile!", "Error!", 0);
		}
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			char * err = (char*)malloc(InfoLogLength+1);
			glGetShaderInfoLog(fs, InfoLogLength, NULL, 
				&err[0]);
			printf("Fragment shader failed to compile! LOG:\n%s\n", 
				&err[0]);
			MessageBox(0, "Fragment shader failed to compile!", "Error!", 0);
		}

		GLuint shader_program = glCreateProgram();
		glAttachShader(shader_program, fs);
		glAttachShader(shader_program, vs);
		glLinkProgram(shader_program);

		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			char * err = (char*)malloc(InfoLogLength+1);
			glGetProgramInfoLog(shader_program, InfoLogLength, NULL, &err[0]);
			printf("SHADERPROGRAM LINK FAILURE! LOG:\n%s\n", &err[0]);
			MessageBox(0, "Shader link failure!", "Error!", 0);
		}

		return shader_program;
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		printf("%i %i\n", width, height);
		glViewport(0, 0, width, height);
	}

	GLFWwindow* window;

	static void error_callback(int error, const char* description){
	    MessageBox(0, description, "Error!", 0);
	}


	GLFWwindow* getWindow(){
		return window;
	}

	const unsigned char* renderer;
	const unsigned char* version ;

	const unsigned char* getRenderer() { return renderer; } ;
	const unsigned char* getVersion() { return version; };


	int init(){
		Logger::log("GRAPHICS :: Initializing...");
		if (!glfwInit())
			return -1;

		glfwSetErrorCallback(error_callback);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, MSAA_SAMPLES);

		char bfr[128];
		sprintf(bfr, "GRAPHICS :: Using %ix MSAA", MSAA_SAMPLES);
		Logger::log(bfr);

		int width = 640, height = 480;
		window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);

		if (!window) {
			printf("Window creation failed!\n");
			glfwTerminate();
			return -1;
		}

		glfwMakeContextCurrent(window);
		glewExperimental = GL_TRUE;
		GLenum game_glewinit = glewInit();

		if(game_glewinit){
			printf("Glew Failed: %s\n", glewGetErrorString(game_glewinit));
			MessageBox(0, "GLEW failed!", "Error!", 0);
			return -1;
		}


		if(!text::init()) return -1;

		renderer = glGetString(GL_RENDERER); 
		version  = glGetString(GL_VERSION); 
		printf("Renderer: %s\n", renderer);
		printf("OpenGL %s\n", version);

		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS); 


		// SHADER
		voxel::init();

		Logger::log("GRAPHICS :: Initialized!");

		return 0;
	}

	bool wireFrame = false;

	void toggleWireframe(){
		wireFrame = !wireFrame;
		glPolygonMode( GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL );
	}

	glm::vec3 screenToWorldSpaceVector(
		int x, int max_x, 
		int y, int max_y, 
		glm::mat4 pRotMat
	){
		glm::vec4 ray_clip = {
			 (x/(float)max_x) * 2.0f - 1.0f,
			-(y/(float)max_y) * 2.0f + 1.0f,
			-1.0f, 1.0f
		};
		glm::vec4 ray = glm::inverse(pRotMat) * ray_clip;
		return glm::normalize(ray);
	}

	void setClearColor(float r, float g, float b, float a){
		glClearColor(r, g, b, a);
	}

	double getTime(){
		return glfwGetTime();
	}

	int windowShouldClose(){
		return glfwWindowShouldClose(window);
	}

	void startFrame(){
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


}