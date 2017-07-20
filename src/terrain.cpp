// Standard
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>
#include <assert.h>

// Third
#include <glm/gtc/noise.hpp>
#include <glm/vec3.hpp>

#include <rdtsc.hpp>


#define FEAT_SIZE 0.01

namespace Terrain {

	typedef struct {
		char * vxl;
		int offset[3];
		int root[3];
	} Terrain;


	Terrain terrain;
	Terrain*getTerrain(){
		return &terrain;
	};


	char read(int l[3]){

		int i = (l[0] + terrain.root[0] * (l[1] + terrain.root[1] * l[2]) );
		return terrain.vxl[i];
	}

	void write(int l[3], char data ){

		int i = (l[0] + terrain.root[0] * (l[1] + terrain.root[1] * l[2]) );
		terrain.vxl[i] = data;
	}

	void generateTerrain(){

		for (int x = 0; x < terrain.root[0]; ++x)
		for (int z = 0; z < terrain.root[2]; ++z)
		{	
			float a = (glm::simplex(glm::vec3(x * 0.005, 0.0f, z * 0.005))+ 1.0 ) / 2.0;
			float b = glm::simplex(glm::vec3(x * 0.1, 0.0f, z * 0.1));
			int h = 
				pow( a, 3 )*350
				+ (b * a +1)*20;
			int w = 20;
			int m = 70;

			// #pragma omp paraller for
			for (int y = 0; y < terrain.root[1]; ++y)
			{	
				int vec2[] = {x, y, z};

				char rgb = 0;;

				if(h>y){
					if   	(h < w+3) rgb = 0b111110;
					else if (h >  m) rgb =  0b010101;
					else			 rgb =  0b000100;
					// char r = (glm::simplex(glm::vec3(x * 0.2, y * 0.2 ,z *0.2))+1.0f)/2.0f * 3.0f;
					// char g = (glm::simplex(glm::vec3(x * 0.1, y * 0.1 ,z *0.1))+1.0f)/2.0f * 3.0f; 
					// char b = (glm::simplex(glm::vec3(x * 0.3, y * 0.3 ,z *0.3))+1.0f)/2.0f * 3.0f;
					// rgb = b | r << 4 | g << 2;
					
				}
				else if(y<w) rgb = 0b011011;

				write(vec2, rgb);
			}

		}
	}

	void generateSimplex(){
		for (int x = 0; x < terrain.root[0]; ++x)
		for (int y = 0; y < terrain.root[1]; ++y)
		for (int z = 0; z < terrain.root[2]; ++z)
		{
			double h = glm::simplex(glm::vec3(
				x * FEAT_SIZE, 
				y * FEAT_SIZE, 
				z * FEAT_SIZE
			));

			int vec2[] = {x, y, z};
			write(vec2,  h>0.0 ? 0b000011 : 0 );

		}
	}

	int simplexBlock(int vec[3]){
		return glm::simplex(glm::vec3((double) vec[0] * FEAT_SIZE, (double) vec[1] * FEAT_SIZE, (double) vec[2] * FEAT_SIZE)) > 0.5 ? 1 : 0;
	}


	void init(int root[3]){
		


		for (int i = 0; i < 3; ++i) terrain.root[i] = root[i];
		printf("[Terrain] Allocating %i*%i*%i blocks;\n", terrain.root[0], terrain.root[1], terrain.root[2]);
		int size = terrain.root[0]*terrain.root[1]*terrain.root[2];
		printf("[Terrain] Total of %i kilobytes\n", size/1024);

		terrain.vxl = (char*)malloc(terrain.root[0]*terrain.root[1]*terrain.root[2]*sizeof(char));

		for (int i = 0; i < 3; ++i) terrain.offset[i] = 0;

		generateTerrain();
	}




}