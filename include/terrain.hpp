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


	int read(int l[3]);
	void generateTerrain();
	void generateSimplex();
}


#endif