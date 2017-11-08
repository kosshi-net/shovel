#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <graphics/driver.h>
#include <graphics/core.hpp>
#include <graphics/cull.hpp>
// using namespace graphics;

#include <core/log.hpp>

#include <mesher/terrainmesher.hpp>

#include "../build/res.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace graphics {
	namespace voxel {


		GLuint shader;
		GLuint shader_uMP;
		GLuint shader_uMV;
		GLuint shader_uFogColor;
		GLuint shader_aVertex;
		GLuint shader_aColor;

		float* planes;

		int init(){


			Logger::log("GRAPHICS :: VOXEL :: Initializing...");
			Logger::log("GRAPHICS :: VOXEL :: Building shaders...");
			shader = createShader( chunkVertSrc, chunkFragSrc );
			glUseProgram(shader);
			shader_aVertex = 		glGetAttribLocation (shader, "aVertex");
			shader_aColor =  		glGetAttribLocation (shader, "aColor");
			shader_uMP =     		glGetUniformLocation(shader, "uMP");
			shader_uMV =     		glGetUniformLocation(shader, "uMV");
			shader_uFogColor =      glGetUniformLocation(shader, "uFogColor");

			planes = Culler::createPlaneBuffer();

			Logger::log("GRAPHICS :: VOXEL :: Initialized!");

			return 1;
		}


		void setFogColor(float r, float g, float b, float a){

			glClearColor(r, g, b, a);

			glUseProgram(shader);

			glUniform3f(shader_uFogColor, r,g,b);
		}


		void meshTick(){
			for (int j = 0; j < 4; ++j){
				TerrainMesher::MeshBuffer* mesh = TerrainMesher::getMesh(1);
				if(mesh == NULL) break;

				// printf("[MeshTick] Mesh done, uploading!\n");

				int i = mesh->chunk;
				TerrainMesher::ChunkD* chunks = TerrainMesher::getChunks();

				// Check if buffers have ever been generated, if no, gen!
				if( chunks[i].state & 0b10 ){
					glGenBuffers(1, &chunks[i].vertexBuffer );
					glGenBuffers(1, &chunks[i].indexBuffer  );
					glGenBuffers(1, &chunks[i].colorBuffer  );
				}
				
				chunks[i].state = chunks[i].state & 0b1;	

				glBindBuffer(GL_ARRAY_BUFFER, chunks[i].vertexBuffer);
				int size = mesh->vertexCount * sizeof(float);
				glBufferData(
					GL_ARRAY_BUFFER, 
					size, 
					mesh->vertexBuffer, 
					GL_STATIC_DRAW
				);

				glBindBuffer(GL_ARRAY_BUFFER, chunks[i].colorBuffer);
				size = mesh->vertexCount * sizeof(float);
				glBufferData(
					GL_ARRAY_BUFFER, 
					size, 
					mesh->colorBuffer, 
					GL_STATIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunks[i].indexBuffer);
				size = mesh->indexCount * sizeof(int);
				glBufferData(
					GL_ELEMENT_ARRAY_BUFFER, 
					size, 
					mesh->indexBuffer, 
					GL_STATIC_DRAW
				);

				chunks[i].items = mesh->indexCount;

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,	0);
				glBindBuffer(GL_ARRAY_BUFFER, 			0);

				TerrainMesher::unlockMesh(mesh);
			}
		}


		void draw(glm::mat4 modelview, glm::mat4 projection){
			glUseProgram(shader);
			glEnableVertexAttribArray(shader_aVertex);
			glEnableVertexAttribArray(shader_aColor);

			glEnable(GL_CULL_FACE); 
			glCullFace(GL_BACK);

			glUniformMatrix4fv(shader_uMV, 1, GL_FALSE, &modelview[0][0]);
			glUniformMatrix4fv(shader_uMP, 1, GL_FALSE, &projection[0][0]);

			meshTick();

			TerrainMesher::ChunkD* chunks = TerrainMesher::getChunks();

			glEnableVertexAttribArray(shader_aVertex);
			glEnableVertexAttribArray(shader_aColor);

			glm::mat4 mvp = projection*modelview;

			Culler::extractPlanes( &mvp[0][0], planes );

			DebugInfo * debugInfo = getDebugInfo();
			
			debugInfo->drawCalls = 0;
			debugInfo->chunksTotal = 0;
			debugInfo->chunksRendered = 0;

			int count = 0;
			for (int i = 0; i < TerrainMesher::getChunkCount(); i++) {
				count++;
				if ( chunks[i].items > 0 ) {

					float floc[] = {
						(float)chunks[i].loc[0]*64,
						(float)chunks[i].loc[1]*64,
						(float)chunks[i].loc[2]*64
					};

					if( 
						!Culler::frustumcull( planes, floc, 64*1.7320508075688772 )
						// Magic number in there is lenght of vector {1,1,1}
					) continue;
					debugInfo->drawCalls++;
					debugInfo->chunksRendered++;
					

					glBindBuffer(GL_ARRAY_BUFFER, chunks[i].colorBuffer );

					glVertexAttribPointer(
						shader_aColor, 3, GL_FLOAT, GL_FALSE, 0, NULL
					);

					glBindBuffer(GL_ARRAY_BUFFER, chunks[i].vertexBuffer);
					glVertexAttribPointer(
						shader_aVertex, 3, GL_FLOAT, GL_FALSE, 0, NULL
					);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunks[i].indexBuffer);

					glDrawElements(
						GL_TRIANGLES, chunks[i].items, GL_UNSIGNED_INT, 0
					);	
				}
			}

			glDisableVertexAttribArray(shader_aVertex);
			glDisableVertexAttribArray(shader_aColor);

			glDisable(GL_CULL_FACE); 
		}


	}
}