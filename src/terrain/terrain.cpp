// Standard
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>
#include <assert.h>

// Third
#include <glm/gtc/noise.hpp>
#include <glm/vec3.hpp>

#include <log/log.hpp>


#define FEAT_SIZE 0.01

namespace Terrain {

	typedef struct {
		char * vxl;
		int offset[3];
		int root[3];
	} Terrain;

	Terrain terrain;
	Terrain * getTerrain(){
		return &terrain;
	};

	// Reads from the volume with an coordinate
	char read(int l[3]){
		int i = (l[0] + terrain.root[0] * (l[1] + terrain.root[1] * l[2]) );
		return terrain.vxl[i];
	}

	void write(int l[3], char data ){
		int i = (l[0] + terrain.root[0] * (l[1] + terrain.root[1] * l[2]) );
		terrain.vxl[i] = data;
	}

	bool inbounds( int l[3]){
		return (
			l[0] < terrain.root[0] 
			&& l[1] < terrain.root[1]
			&& l[2] < terrain.root[2] 
			&& l[0] > -1 && l[1] > -1 && l[2] > -1 
		);
	};

	char saferead(int l[3]){
		if(inbounds(l)) 
			return read(l);
		return 0;
	}


	void generateTerrain(){
		#pragma omp parallel for
		for (int x = 0; x < terrain.root[0]; ++x)
		for (int z = 0; z < terrain.root[2]; ++z)
		{	
			float a = (glm::simplex(glm::vec3((x+9001) * 0.0050, 0.0f,   (z+9001) * 0.0050)) + 1.0 ) / 2.0;
			float b = (glm::simplex(glm::vec3((x+9001) * 0.1000, 0.0f,   (z+9001) * 0.1000)) + 1.0 ) / 2.0;
			float c = (glm::simplex(glm::vec3((x+9001) * 0.0010, 0.0f,   (z+9001) * 0.0010)) + 1.0 ) / 2.0;
			int h = 
				pow( a*c, 2.0 )*300+15
				+ pow(b*a, 2.0)*50;
			int w = 20;
			int m = 70;
			for (int y = 0; y < terrain.root[1]; ++y)
			{	
				int vec2[] = {x, y, z};
				char rgb = 0;
				if(h>y){
					if   	(h < w+3) rgb = 0b111110;
					else if (h >  m) rgb =  0b010101;
					else			 rgb =  0b000100;
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


	bool raycast(
		float vector[], float origin[],
		float * distance, int * hitloc, int * normal, char * block
	){

		float coord[3];
		for (int i = 0; i < 3; ++i) coord[i] = floorf(origin[i]);
		float deltaDist[3];
		float next[3];
		float step[3];
	

		float t[3][3];

		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j) t[i][j] = vector[j] / vector[i];

		for (int i = 0; i < 3; ++i) {
			deltaDist[i] = sqrt( t[i][0]*t[i][0] + t[i][1]*t[i][1] + t[i][2]*t[i][2] );

			if (vector[i] < 0) {
				step[i] = -1.0f;
				next[i] = (origin[i] - coord[i]) * deltaDist[i];
			} else {
				step[i] = 1.0f;
				next[i] = (coord[i] + 1.0f - origin[i]) * deltaDist[i];
			}
		}


		while (*distance > 0.0f) {
			int side = 0;

			for (int j = 1; j < 3; ++j) {
				if (next[side] > next[j]) {
					side = j;
				}
			}

			next[side] += deltaDist[side];
			coord[side] += step[side];

			*distance -= fabs( step[side] );

			for (int i = 0; i < 3; ++i) hitloc[i] = coord[i];

			*block = saferead(hitloc);
			if( *block > 0 ){
				
				normal[side] = step[side];

				return true;
			}
		}
		return false;
	}

	void init(int root[3]){
		for (int i = 0; i < 3; ++i) terrain.root[i] = root[i];
		int size = terrain.root[0]*terrain.root[1]*terrain.root[2];

		char txtbfr[128];

		snprintf(txtbfr, sizeof(txtbfr), "TERRAIN :: Allocating %i*%i*%i blocks (%i MB);", terrain.root[0], terrain.root[1], terrain.root[2], size/1024/1024);

		Logger::log(txtbfr);

		terrain.vxl = (char*)malloc(terrain.root[0]*terrain.root[1]*terrain.root[2]*sizeof(char));

		for (int i = 0; i < 3; ++i) terrain.offset[i] = 0;

		generateTerrain();
	}
}