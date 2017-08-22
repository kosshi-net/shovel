#ifndef KOSSHI_TERRAINMESHER_INCLUDED
#define KOSSHI_TERRAINMESHER_INCLUDED 1

#include <terrain.hpp>

namespace TerrainMesher {

	typedef struct {
		float* vertexBuffer;
		unsigned int* indexBuffer;
		float* colorBuffer;
		
		short* 			maskBuffer;

		int vertexCount;
		int indexCount;
		int chunk;
		int locked;
	} MeshBuffer;


	typedef struct {
		int count;
		int root;
		int * x;
		int * y;
		int * z;
		unsigned int * vertexBuffer;
		unsigned int * indexBuffer;
		unsigned int * colorBuffer;
		int * state;
		unsigned int * items;
	} ChunkList;

	ChunkList * getChunks();

	void init(int[], int);
	MeshBuffer* getMesh(int);
	void unlockMesh(MeshBuffer*);

	float getActivity();
	int getCount();

	void markDirty(int*);
}

#endif