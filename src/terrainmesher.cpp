// Standard
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>
#include <assert.h>
#include <pthread.h>
#include <algorithm>

#include <xsleep.hpp>
#include <terrain.hpp>

#include <luts.hpp>

namespace TerrainMesher {

	typedef struct {
		float*			vertexBuffer;
		unsigned int*   indexBuffer;
		float* 			colorBuffer;

		short* 			maskBuffer;
		int vertexCount;
		int indexCount;
		int chunk;
	} MeshBuffer;



	typedef struct {
		int count;
		int root;
		int * x; int * y; int * z;
		unsigned int * vertexBuffer;
		unsigned int * indexBuffer;
		unsigned int * colorBuffer;
		int * state;
		unsigned int * items;
	} ChunkList;

	ChunkList chunks;

	enum ThreadState {
		START,
		WORK,
		SLEEP,
		COMMAND_WAKE,
		COMMAND_KILL
	};

	pthread_t thread;
	MeshBuffer mesh;
	ThreadState threadState = START;

	ChunkList * getChunks(){
		return &chunks;
	};


	void rgb8bit ( float * rgb, char n ) {
		rgb[0] = (n >> 4 & 0b11)/3.0f;
		rgb[1] = (n >> 2 & 0b11)/3.0f;
		rgb[2] = (n      & 0b11)/3.0f;
	};

	int vertexAO(int side1, int side2, int corner) {
		if(side1 && side2) return 0;
		return (3 - (side1 + side2 + corner));
	};

	int getSliceIndex( int root, int dimension, int slice){
		int items = root*root;
		int slices = root;
		
		return (items*slices) * dimension + (items * slice);
	}

	int itemIndex(int root, int dim, int * l){
		switch (dim){
			case 0: return l[1]*root + l[2];
			case 1: return l[0]*root + l[2];
			case 2: return l[1]*root + l[0];
		}
		return 0;
	}

	short encodeMaskItem(int facing, int inverse, int color6bit, int*ao){
		short value = 0;
		// AOAOAOAOFMRRGGBB
		value = color6bit & 0b00111111;
		value = value | (inverse << 6);
		value = value | (facing  << 7);

		for (int i = 0; i < 4; ++i) {
			value = value | ( ao[i] << (8 + i*2) );
		}
		return value;
	}

	void decodeMaskItem(short value, int*facing, int*inverse, int*color6bit, int*ao){
		*color6bit = value & 0b00111111;
		*inverse = (value >> 6) & 1;
		*facing  = (value >> 7) & 1;
		
		for (int i = 0; i < 4; ++i) {
			ao[i] = (value >> (8 + i*2)) & 0b11;
		}
	}

		// #pragma omp parallel for
	void mesher(Terrain::Terrain * terrain, int offset[3], int chunkroot, MeshBuffer*mesh){
		
		#pragma omp parallel for
		for (int i0 = 0; i0 < chunkroot; ++i0)
		for (int i1 = 0; i1 < chunkroot; ++i1)
		for (int i2 = 0; i2 < chunkroot; ++i2)
		{	
			int AC[3] = {i2, i1, i0};

		 	// AC = chunk location, AW = world location
			int AW[3]; 
			for (int i = 0; i < 3; ++i) AW[i] = offset[i] + AC[i];

			int a = 0;					 // inlined bound check.
			if( AW[0] < terrain->root[0] // saferead checks 6 sides
			 && AW[1] < terrain->root[1] // we dont need to check
			 && AW[2] < terrain->root[2] // if under 0, so its inlined
			) a = Terrain::read(AW);

			for (int i = 0; i < 3; ++i) {  // face loop

				int BW[3]; 
				for (int i = 0; i < 3; ++i) BW[i] = AW[i]; 

				BW[i]--;
				int b = Terrain::saferead(BW);

				if ((a==0)==(b==0)) continue; // culling

				int facing, color6bit;

				if (b==0) {
					facing = 1;
					color6bit = a;
				} else {
					facing = 0;
					color6bit = b;
				}

				BW[i]+=(b>0);

				int computedAO[4];

				for (int j = 0; j < 4; ++j){ // Ambient occlusion mess
					int l[3];
					for (int s = 0; s < 3; ++s) l[s] = BW[s] + aolut[facing][i][j][0][s];
					int side1  = (Terrain::saferead( l ) > 0);
					for (int s = 0; s < 3; ++s) l[s] = BW[s] + aolut[facing][i][j][1][s];
					int corner = (Terrain::saferead( l ) > 0);
					for (int s = 0; s < 3; ++s) l[s] = BW[s] + aolut[facing][i][j][2][s];
					int side2  = (Terrain::saferead( l ) > 0);
					computedAO[j] = vertexAO( side1>0, side2>0, corner>0 );
				}
				int inverse = 0;
				if( computedAO[1] + computedAO[3] < computedAO[0] + computedAO[2] ) 
					inverse = 1;

				// Push result into a mask for greedy meshing
				int maskIndex = getSliceIndex(chunkroot, i, AC[i]) + itemIndex(chunkroot, i, AC);
				mesh->maskBuffer[maskIndex] = encodeMaskItem(facing, inverse, color6bit, computedAO);
			}
		}


		int vertexIndex = 0;
		int indexIndex = 0;
		int vertexCount = 0;

		// GREEDY ALGO
		for (int dim = 0; dim < 3; ++dim) // dim
		for (int slice = 0; slice < chunkroot; ++slice){
			int __itemIndex = 0;
			for (int u = 0; u < chunkroot; ++u)
			for (int v = 0; v < chunkroot; ++v){

				int sliceIndex = getSliceIndex(chunkroot, dim, slice);
				int itemIndex = __itemIndex;
				__itemIndex++;

				short itemData = mesh->maskBuffer[ sliceIndex+itemIndex ];

				if(itemData==0) continue;

				int facing;
				int inverse;
				int color6bit;
				int ao[4];
				decodeMaskItem(itemData, &facing, &inverse, &color6bit, ao);

				// EXPANDER

				int sizeU = 0;
				while(true){
					mesh->maskBuffer[ sliceIndex+itemIndex+sizeU ] = 0;
					sizeU++;
					if( v + sizeU >= chunkroot ) break;
					if( mesh->maskBuffer[ sliceIndex+itemIndex+sizeU ] != itemData ) break;
				}

				int sizeV = 0;
				while(true){

					sizeV++;
					if( u + sizeV >= chunkroot ) break;

					bool expandable = true;

					for (int i = 0; i < sizeU; ++i) {
						if( mesh->maskBuffer[ sliceIndex+itemIndex + chunkroot*sizeV + i ] == itemData ) continue;
						expandable = false;
						break;
					}

					if(expandable)
						for (int i = 0; i < sizeU; ++i) mesh->maskBuffer[ sliceIndex+itemIndex + chunkroot*sizeV + i ] = 0;
					else 
						break;
				}

				float rgb[3];
				rgb8bit(rgb, color6bit);

				int scale[] = { 1, 1, 1 };
				int BW[3];
				for (int i = 0; i < 3; ++i) BW[i] = offset[i];
				switch (dim) {
					case 0:
						BW[0]+= slice;
						BW[1]+= u;
						BW[2]+= v;

						scale[2] = sizeU;
						scale[1] = sizeV;
						break;
					case 1:
						BW[0]+= u;
						BW[1]+= slice;
						BW[2]+= v;

						scale[2] = sizeU;
						scale[0] = sizeV;
						break;
					case 2:
						BW[0]+= v;
						BW[1]+= u;
						BW[2]+= slice;

						scale[0] = sizeU;
						scale[1] = sizeV;
						break;
				}

				if(!facing) BW[dim]--;

				int side = dim;
				if(facing) side += 3;

				for (int j = 0; j < 4; ++j)
				for (int k = 0; k < 3; ++k){
					mesh->vertexBuffer[vertexIndex]  = vertices[side][j*3+k] * scale[k] + BW[k];
					mesh->colorBuffer[vertexIndex++] =
						rgb[k] * ( 0.8 + pow( (float)ao[j] / 3.0, 2) * 0.2) * colors[side][j*3+k];
				}
				
				for (int i = 0; i < 6; ++i) mesh->indexBuffer[indexIndex++] = indices[inverse][i] + vertexCount;
				vertexCount+=4;
			}

			mesh->vertexCount = vertexIndex;
			mesh->indexCount = indexIndex;
		}
	}



	static void* threadMain(void*arg){
		using namespace TerrainMesher;
		int i = 0;

		int bsize = 8*1024*1024;
		mesh.vertexBuffer = (float*)		malloc(bsize*sizeof(float));
		mesh.colorBuffer  = (float*)		malloc(bsize*sizeof(float));
		mesh.indexBuffer  = (unsigned int*)	malloc(bsize*sizeof(int));

		int maskBufferSize = 3 * chunks.root * (chunks.root*chunks.root) * sizeof(short);


		mesh.maskBuffer   = (short*)		calloc( maskBufferSize, 1 );

		printf("[TerraMesher-T1] vertexGeometryBuffer: %iKB\n", bsize*sizeof(float)/1024);
		printf("[TerraMesher-T1] ColorGeometryBuffer: %iKB\n", bsize*sizeof(float)/1024);
		printf("[TerraMesher-T1] IndexGeometryBuffer: %iKB\n", bsize*sizeof(int)/1024);
		printf("[TerraMesher-T1] MaskBuffers: %iKB\n", maskBufferSize/1024);



		while(threadState != COMMAND_KILL) {

			if( i == chunks.count) {
				i = 0;
			}

			if (threadState == SLEEP) {
				xsleep(0);
				continue;
			}

			threadState = WORK;

			int l[] = {
				chunks.x[i]*chunks.root,
				chunks.y[i]*chunks.root,
				chunks.z[i]*chunks.root
			};

			mesh.chunk = i;

			if(chunks.state[i] & 0b1) {
				chunks.state[i] = chunks.state[i] ^ 0b1;
				clock_t start = clock();
				mesher( Terrain::getTerrain(), l, chunks.root, &mesh);
				clock_t end = clock();
				float seconds = (float)(end - start) / CLOCKS_PER_SEC * 1000;
				printf("%i ms, %i\n", (int)seconds, mesh.indexCount);
				// if(mesh.indexCount == 0){
				// 	chunks.state[i] = 1;
				// } else {
				// 	threadState = SLEEP;
				// }
				threadState = SLEEP;
			}
			i++;
		}
		return NULL;
	}

	MeshBuffer * getMesh(){
		if(threadState == SLEEP){
			return &mesh;
		}
		return NULL;
	}

	void meshUsed(){
		if(threadState == SLEEP)
			threadState = COMMAND_WAKE;
		else
			printf("ERROR");
	}


	void init(int terrainroot[3], int chunkroot){
		chunks.count = (terrainroot[0]/chunkroot) * (terrainroot[1]/chunkroot) * (terrainroot[2]/chunkroot);
		chunks.root = chunkroot;
		printf("[TerraMesher] Allocating %i CVOs...\n", chunks.count);
		chunks.x =				(int*)			malloc(sizeof(int) * chunks.count );
		chunks.y =				(int*)			malloc(sizeof(int) * chunks.count );
		chunks.z =				(int*)			malloc(sizeof(int) * chunks.count );
		chunks.vertexBuffer =	(unsigned int*)	malloc(sizeof(int) * chunks.count );
		chunks.indexBuffer =	(unsigned int*)	malloc(sizeof(int) * chunks.count );
		chunks.colorBuffer =	(unsigned int*)	malloc(sizeof(int) * chunks.count );
		chunks.items =			(unsigned int*)	malloc(sizeof(int) * chunks.count );
		chunks.state =			(int*)			malloc(sizeof(int) * chunks.count );

		// 0 - unintialized
		// 1 - done
		// 2 - dirty

		// 1 << 1 Dirty
		// 1 << 0 BufferCreated




		printf("[TerraMesher] Filling...\n");
		int i = 0;
		for (int x = 0; x < terrainroot[0]/chunkroot; ++x)
		for (int y = 0; y < terrainroot[1]/chunkroot; ++y)
		for (int z = 0; z < terrainroot[2]/chunkroot; ++z)
		{
			chunks.x[i]=x;
			chunks.y[i]=y;
			chunks.z[i]=z;

			chunks.state[i] = 0b11;
			chunks.items[i] = 0;

			i++;
		}


		
		pthread_create( &thread, NULL, &threadMain, NULL);
		printf("[TerrainMesher] Mesher thread started!\n");
	}

	void markDirtySingle(int l[]){
		int cl[3];

		for (int i = 0; i < 3; ++i) cl[i] = l[i]/chunks.root;

		for (int i = 0; i < chunks.count; ++i) {
			if(
				cl[0] == chunks.x[i] &&
				cl[1] == chunks.y[i] &&
				cl[2] == chunks.z[i]
			){	
				chunks.state[i] = chunks.state[i] | 0b1;
				// printf("Marked %i as dirty\n", i);
				return;
			}
		}
	}

	void markDirty(int l[]){

		int m[3] = {0};
		for (m[0] = l[0]-1; m[0] < l[0]+2; ++m[0])
		for (m[1] = l[1]-1; m[1] < l[1]+2; ++m[1])
		for (m[2] = l[2]-1; m[2] < l[2]+2; ++m[2])
		{
			markDirtySingle(m);
		}
	}

}