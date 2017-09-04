#ifndef KOSSHI_TERRAIN_INCLUDED
#define KOSSHI_TERRAIN_INCLUDED 1


namespace Terrain {
	typedef struct {
		char * vxl;
		int offset[3];
		int root[3];
	} Terrain;


	void init(int root[]);

	Terrain*getTerrain();


	char read(int l[3]);
	void write(int l[3], char);
	char saferead(int l[3]);
	bool inbounds(int*);
	bool raycast(
		float vector[], float origin[],
		float*distance, int * hitloc, int * normal, char * block
	);



	void generateTerrain();
	void generateSimplex();
}


#endif