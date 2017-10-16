// Standard
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>
#include <assert.h>
#include <pthread.h>
#include <algorithm>

#include <core/xsleep.hpp>
#include <terrain/terrain.hpp>

#include <core/luts.hpp>

#include <core/log.hpp>
#include <event/event.hpp>

#define MESH_BUFFER_COUNT 4
#define MESH_BUFFER_SIZE (64*64*64)/2*6*4;

namespace TerrainMesher {

	typedef struct {
		float*			vertexBuffer;
		unsigned int*   indexBuffer;
		float* 			colorBuffer;

		short* 			maskBuffer;
		int vertexCount;
		int indexCount;
		int chunk;

		int locked; 
		// locked write, means that the buffer has not been uploaded yet
	} MeshBuffer;
	// Meshbuffer is the object where the mesh is written. Temporary store
	// until uploaded to gpu. 

	typedef struct {
		int loc[3];
		unsigned int vertexBuffer;
		unsigned int indexBuffer;
		unsigned int colorBuffer;
		int state;
		unsigned int items;
	} ChunkD;


	ChunkD * chunks;
	int chunkCount = 0;
	int chunkroot = 0;


	enum ThreadState {
		START,
		WORK,
		SLEEP,
		COMMAND_WAKE,
		COMMAND_KILL
	};

	pthread_t thread;

	MeshBuffer mesh[MESH_BUFFER_COUNT];

	ThreadState threadState = START;

	ChunkD * getChunks(){
		return chunks;
	};
	int getChunkCount(){
		return chunkCount;
	}


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

	void decodeMaskItem(
		short value, int*facing, int*inverse, int*color6bit, int*ao
	){
		*color6bit = value & 0b00111111;
		*inverse = (value >> 6) & 1;
		*facing  = (value >> 7) & 1;
		
		for (int i = 0; i < 4; ++i) {
			ao[i] = (value >> (8 + i*2)) & 0b11;
		}
	}

	void mesher(
		Terrain::Terrain * terrain, 
		int offset[3],
		int chunkroot, 
		MeshBuffer*mesh
	){
		
		bool hasBlocks = false;
		#pragma omp parallel for
		for (int i0 = 0; i0 < chunkroot; ++i0)
		for (int i1 = 0; i1 < chunkroot; ++i1)
		for (int i2 = 0; i2 < chunkroot; ++i2)
		{	
			int AC[3] = {i1, i2, i0};

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

				if(!hasBlocks) hasBlocks = true;

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
					for (int s = 0; s < 3; ++s) 
						l[s] = BW[s] + aolut[facing][i][j][0][s];
					int side1  = (Terrain::saferead( l ) > 0);
					for (int s = 0; s < 3; ++s) 
						l[s] = BW[s] + aolut[facing][i][j][1][s];
					int corner = (Terrain::saferead( l ) > 0);
					for (int s = 0; s < 3; ++s) 
						l[s] = BW[s] + aolut[facing][i][j][2][s];
					int side2  = (Terrain::saferead( l ) > 0);
					computedAO[j] = vertexAO( side1>0, side2>0, corner>0 );
				}
				int inverse = 0;
				if( computedAO[1]+computedAO[3] < computedAO[0]+computedAO[2] ) 
					inverse = 1;

				// Push result into a mask for greedy meshing
				int maskIndex = getSliceIndex(
					chunkroot, i, AC[i]) + itemIndex(chunkroot, i, AC
				);
				mesh->maskBuffer[maskIndex] = encodeMaskItem(
					facing, inverse, color6bit, computedAO
				);
			}
		}


		int vertexIndex = 0;
		int indexIndex = 0;
		int vertexCount = 0;

		// GREEDY ALGO
		if(hasBlocks)
		for (int slice = 0; slice < chunkroot; ++slice)
		for (int dim = 0; dim < 3; ++dim)
		{
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

					if( mesh->maskBuffer[ 
						sliceIndex + itemIndex + sizeU 
						] != itemData 
					) break;
				}

				int sizeV = 0;
				while(true){

					sizeV++;
					if( u + sizeV >= chunkroot ) break;

					bool expandable = true;

					for (int i = 0; i < sizeU; ++i) {
						if( mesh->maskBuffer[ 
							sliceIndex+itemIndex + chunkroot*sizeV + i 
							] == itemData 
						) continue;
						expandable = false;
						break;
					}

					if(expandable)
						for (int i = 0; i < sizeU; ++i) 
							mesh->maskBuffer[ 
								sliceIndex+itemIndex + chunkroot*sizeV + i 
							] = 0;
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
					mesh->vertexBuffer[vertexIndex] = 
						vertices[side][j*3+k] * scale[k] + BW[k];

					mesh->colorBuffer[vertexIndex++] =
						rgb[k] * ( 
							0.8 + pow( (float)ao[j] / 3.0, 2) * 0.2
						) * colors[side][j*3+k];
				}
				
				for (int i = 0; i < 6; ++i) 
					mesh->indexBuffer[indexIndex++] = 
						indices[inverse][i] + vertexCount;

				vertexCount+=4;
			}
		}
		mesh->vertexCount = vertexIndex;
		mesh->indexCount = indexIndex;

	}

	MeshBuffer * getMesh(int locked){
		for (int i = 0; i < MESH_BUFFER_COUNT; ++i) 
			if(mesh[i].locked == locked) {
				return &mesh[i];
			}
		// if(locked) printf("EMPTY\n"); else printf("FULL\n");
		return NULL;
	}


	void lockMesh(MeshBuffer*m){
		m->locked = 1;
	}

	void unlockMesh(MeshBuffer*m){
		m->locked = 0;
	}

	float activity = 0;
	int meshedLastS = 0;

	float getActivity(){
		return activity;
	}
	int getCount(){
		return meshedLastS;
	}

	static void* threadMain(void*arg){
		using namespace TerrainMesher;
		int i = 0;	


		static char txtbfr[256];

		int bsize = MESH_BUFFER_SIZE;


		snprintf( txtbfr, sizeof(txtbfr), 
			"MESHERTHREAD :: Attemtping to allocate %i MB...", 
			(	(bsize*sizeof(float)) *3 + 
				(chunkroot * (chunkroot*chunkroot)
				* sizeof(short))
			)*MESH_BUFFER_COUNT / 1024 / 1024 );

		Logger::log(txtbfr);

		for (int j = 0; j < MESH_BUFFER_COUNT; ++j) {
			mesh[j].vertexBuffer = (float*)		  malloc(bsize*sizeof(float));
			mesh[j].colorBuffer  = (float*)		  malloc(bsize*sizeof(float));
			mesh[j].indexBuffer  = (unsigned int*)malloc(bsize*sizeof(int)*1.5);

			int maskBufferSize = 
				3 * chunkroot * (chunkroot*chunkroot) * sizeof(short);

			mesh[j].maskBuffer   = (short*)		calloc( maskBufferSize, 1 );

			assert( mesh[j].vertexBuffer != NULL );
			assert( mesh[j].colorBuffer != NULL );
			assert( mesh[j].indexBuffer != NULL );
			assert( mesh[j].maskBuffer != NULL );

			mesh[j].locked = 0;

		}

		Logger::log("MESHERTHREAD :: Done allocating!");

		clock_t lastClock = clock();
		clock_t activeTime = 0;

		int count = 0;
		int j = 0;
		while(threadState != COMMAND_KILL) {

			MeshBuffer*m = getMesh( 0 );

			if(m == NULL) {
				Logger::warn("MESHER :: THREAD :: Buffer full, sleeping...");
				xsleep(10);
				continue;
			}

			if(lastClock + CLOCKS_PER_SEC < clock()){
				lastClock += CLOCKS_PER_SEC;
				activity = activeTime / (float)CLOCKS_PER_SEC;

				activeTime=0;

				meshedLastS = count;
				count = 0;

			}

			if( i == chunkCount) i = 0;

			if(chunks[i].state & 0b1) {
				clock_t start = clock();
				m->chunk = i;
				int l[3];
				for (int j = 0; j < 3; ++j)
					l[j] = chunks[i].loc[j]*chunkroot;

				chunks[i].state = chunks[i].state ^ 0b1;
				mesher( Terrain::getTerrain(), l, chunkroot, m);
				count++;
				lockMesh(m);
				clock_t end = clock();
				// int seconds = (float)(end - start) / CLOCKS_PER_SEC * 1000;
				// printf("%i ms, %i\n", (int)seconds, m->indexCount);
				activeTime += (end-start);
			} else j++;
			i++;
			

			if(j > chunkCount){ 
				j = 0;
				xsleep(20); 
			}
		
		}
		Logger::log( "MESHERTHREAD :: Shutting down.." );
		return NULL;
	}

	int activityTimeMS = 0;


	void closeThread(void*n){
		threadState = COMMAND_KILL;
		pthread_join(thread, NULL);
		Logger::log("MESHER :: Thread closed");
	}


	void init(int terrainroot[3], int _chunkroot){
		chunkroot = _chunkroot;
		
		chunkCount = 
			(terrainroot[0]/chunkroot) * 
			(terrainroot[1]/chunkroot) * 
			(terrainroot[2]/chunkroot);


		char txtbfr[128];



		snprintf( txtbfr, sizeof(txtbfr), 
			"MESHER :: Allocating %i chunk objects, %i B  ...", 
			chunkCount, chunkCount*sizeof(int)*8
		);
		Logger::log(txtbfr);

		chunks = (ChunkD*)calloc( sizeof(ChunkD)*chunkCount, 1 );

		// 0 - unintialized
		// 1 - done
		// 2 - dirty

		// 1 << 1 Dirty
		// 1 << 0 BufferCreated

		Logger::log("MESHER :: Filling...");
		int i = 0;
		for (int x = 0; x < terrainroot[0]/chunkroot; ++x)
		for (int y = 0; y < terrainroot[1]/chunkroot; ++y)
		for (int z = 0; z < terrainroot[2]/chunkroot; ++z)
		{
			chunks[i].loc[0]=x;
			chunks[i].loc[1]=y;
			chunks[i].loc[2]=z;

			chunks[i].state = 0b11;
			chunks[i].items = 0;

			i++;
		}

		pthread_create( &thread, NULL, &threadMain, NULL);
		Logger::log("MESHER :: Mesher thread started!");

		EventSystem::bind( EventSystem::EVENT_SHUTDOWN, *closeThread );
	}

	void markDirtySingle(int l[]){
		int cl[3];

		for (int i = 0; i < 3; ++i) cl[i] = l[i]/chunkroot;

		for (int i = 0; i < chunkCount; ++i) {
			if(
				cl[0] == chunks[i].loc[0] &&
				cl[1] == chunks[i].loc[1] &&
				cl[2] == chunks[i].loc[2]
			){	
				chunks[i].state = chunks[i].state | 0b1;
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