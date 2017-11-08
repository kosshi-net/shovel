#include <graphics/driver.h>
#include <graphics/core.hpp>
// #include <graphics/cull.hpp>
// using namespace graphics;

#include <core/log.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../build/res.h"


namespace graphics {
	namespace box {

		GLuint shader;
		GLint shader_aVertex;
		GLint shader_uColor;
		GLint shader_uMP;
		GLint shader_uMV;

		GLuint geom_vbo;

		float geom[] = {
			0,0,0,
			1,0,0,

			1,0,0,
			1,1,0,

			1,1,0,
			0,1,0,

			0,1,0,
			0,0,0,

			0,0,1,
			1,0,1,

			1,0,1,
			1,1,1,

			1,1,1,
			0,1,1,

			0,1,1,
			0,0,1,

			0,0,0,
			0,0,1,

			1,0,0,
			1,0,1,

			1,1,0,
			1,1,1,

			0,1,0,
			0,1,1
		};

		float color[] = { 0,0,0 };

		int init(){
			// for (int i = 0; i < 24*3; ++i)
			// {
			// 	geom[i]*=2;
			// }

			shader = createShader( boxVertSrc, boxFragSrc );
			glUseProgram(shader);
			shader_aVertex =  glGetAttribLocation (shader, "aVertex");

			shader_uColor =  glGetUniformLocation(shader, "uColor");

			shader_uMP =     glGetUniformLocation(shader, "uMP");
			shader_uMV =     glGetUniformLocation(shader, "uMV");

			assert(shader_aVertex != -1);
			assert(shader_uColor != -1);
			assert(shader_uMV 	 != -1);
			assert(shader_uMP 	 != -1);

			glEnableVertexAttribArray(shader_aVertex);

			glGenBuffers(1, &geom_vbo);
			glEnableVertexAttribArray(shader_aVertex);

			glBindBuffer(GL_ARRAY_BUFFER, geom_vbo);
			// glVertexAttribPointer(shader_aCoord, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glBufferData(GL_ARRAY_BUFFER, sizeof(geom), 
				geom, GL_STATIC_DRAW);

			glDisableVertexAttribArray(shader_aVertex);
			Logger::log("GRAPHICS :: BOX :: Initialized");
			return 1;
		}

		void draw_wireframe_cube(glm::mat4 modelview, glm::mat4 projection){
			glUseProgram(shader);
			glEnableVertexAttribArray(shader_aVertex);
			glBindBuffer(GL_ARRAY_BUFFER, geom_vbo);

			glUniformMatrix4fv(shader_uMV, 1, GL_FALSE, &modelview[0][0]);
			glUniformMatrix4fv(shader_uMP, 1, GL_FALSE, &projection[0][0]);
			// glUniform3fv(shader_uColor, 1, &color[0]);

			glVertexAttribPointer(shader_aVertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glDrawArrays(GL_LINES, 0, 24);

			glDisableVertexAttribArray(shader_aVertex);
		}
	}
}
