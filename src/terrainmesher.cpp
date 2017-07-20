// Standard
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>
#include <assert.h>

#include <pthread.h>


#include <xsleep.hpp>

#include <terrain.hpp>



namespace TerrainMesher {

	typedef struct {
		float* vertexBuffer;
		unsigned int* indexBuffer;
		float* colorBuffer;
		
		int vertexCount;
		int indexCount;
		int chunk;
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

	const float vertices[6][12] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,

		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	const unsigned short indices[2][6] = {
		0, 3, 1,
		2, 1, 3,

		3, 2, 0,
		1, 0, 2
	};

	const float colors[6][12] = {
		0.8f, 0.8f, 0.8f,
		0.8f, 0.8f, 0.8f,
		0.8f, 0.8f, 0.8f,
		0.8f, 0.8f, 0.8f,

		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		
		0.8f, 0.8f, 0.8f,
		0.8f, 0.8f, 0.8f,
		0.8f, 0.8f, 0.8f,
		0.8f, 0.8f, 0.8f,

		0.8f, 0.8f, 0.8f,
		0.8f, 0.8f, 0.8f,
		0.8f, 0.8f, 0.8f,
		0.8f, 0.8f, 0.8f,

		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		
		0.8f, 0.8f, 0.8f,
		0.8f, 0.8f, 0.8f,
		0.8f, 0.8f, 0.8f,
		0.8f, 0.8f, 0.8f,
	};


	// int aolut[3][4][3][3] = {
	// 	{
	// 	 {{ 0, 0, 1},{ 0,-1, 1 },{ 0,-1, 0 }},
	// 	 {{ 0,-1, 0},{ 0,-1,-1 },{ 0, 0,-1 }},
	// 	 {{ 0, 0,-1},{ 0, 1,-1 },{ 0, 1, 0 }},
	// 	 {{ 0, 1, 0},{ 0, 1, 1 },{ 0, 0, 1 }},
	// 	},{
	// 	 {{ 1, 0, 0 },{ 1, 0, 1 },{ 0, 0, 1}},
	// 	 {{ 0, 0, 1 },{ 1, 0, 1 },{ 1, 0, 0}},
	// 	 {{ 1, 0, 0 },{ 1, 0,-1 },{ 0, 0,-1}},
	// 	 {{ 0, 0,-1 },{ 1, 0,-1 },{ 1, 0, 0}},
	// 	},{
	// 	 {{ 1, 0, 0 },{ 1,-1, 0 },{ 0,-1, 0 }},
	// 	 {{ 0,-1, 0 },{ 1,-1, 0 },{ 1, 0, 0 }},
	// 	 {{ 1, 0, 0 },{ 1, 1, 0 },{ 0, 1, 0 }},
	// 	 {{ 0, 1, 0 },{ 1, 1, 0 },{ 1, 0, 0 }},
	// 	}
	// };


	// SIDE, VERTEX, SIDE/CORNER/SIDE, OFFSET
	int aolut[2][3][4][3][3] = 
	{
		{
			{
			 {{ 0, 0,-1 },{ 0,-1,-1 },{ 0,-1, 0 }},
			 {{ 0,-1, 0 },{ 0,-1, 1 },{ 0, 0, 1 }},
			 {{ 0, 0, 1 },{ 0, 1, 1 },{ 0, 1, 0 }},
			 {{ 0, 1, 0 },{ 0, 1,-1 },{ 0, 0,-1 }},
			},{
			 {{-1, 0, 0 },{-1, 0,-1 },{ 0, 0,-1 }},
			 {{ 0, 0,-1 },{ 1, 0,-1 },{ 1, 0, 0 }},
			 {{ 1, 0, 0 },{ 1, 0, 1 },{ 0, 0, 1 }},
			 {{ 0, 0, 1 },{-1, 0, 1 },{-1, 0, 0 }},
			},{
			 {{ 1, 0, 0 },{ 1,-1, 0 },{ 0,-1, 0 }},
			 {{ 0,-1, 0 },{-1,-1, 0 },{-1, 0, 0 }},
			 {{-1, 0, 0 },{-1, 1, 0 },{ 0, 1, 0 }},
			 {{ 0, 1, 0 },{ 1, 1, 0 },{ 1, 0, 0 }},
			},
		},{
			{
				// 0 -1 0

			 {{ 0, 0, 1 },{ 0,-1, 1 },{ 0,-1, 0 }},
			 {{ 0,-1, 0 },{ 0,-1,-1 },{ 0, 0,-1 }},
			 {{ 0, 0,-1 },{ 0, 1,-1 },{ 0, 1, 0 }},
			 {{ 0, 1, 0 },{ 0, 1, 1 },{ 0, 0, 1 }},
			},{
			 {{-1, 0, 0 },{-1, 0, 1 },{ 0, 0, 1 }},
			 {{ 0, 0, 1 },{ 1, 0, 1 },{ 1, 0, 0 }},
			 {{ 1, 0, 0 },{ 1, 0,-1 },{ 0, 0,-1 }},
			 {{ 0, 0,-1 },{-1, 0,-1 },{-1, 0, 0 }},
			},{
			 {{-1, 0, 0 },{-1,-1, 0 },{ 0,-1, 0 }},
			 {{ 0,-1, 0 },{ 1,-1, 0 },{ 1, 0, 0 }},
			 {{ 1, 0, 0 },{ 1, 1, 0 },{ 0, 1, 0 }},
			 {{ 0, 1, 0 },{-1, 1, 0 },{-1, 0, 0 }},
			}
		}
	};



	void rgb8bit ( float * rgb, char n ) {
		rgb[0] = (n >> 4 & 0b11)/3.0f;
		rgb[1] = (n >> 2 & 0b11)/3.0f;
		rgb[2] = (n      & 0b11)/3.0f;
	};

	int vertexAO(int side1, int side2, int corner) {
		if(side1 && side2) {
			return 0;
		}
		return (3 - (side1 + side2 + corner));
	};

	bool inbounds(Terrain::Terrain * terrain, int l[3]){
		return (	l[0] < terrain->root[0] 
				 && l[1] < terrain->root[1]
				 && l[2] < terrain->root[2] 
				 && l[0] > -1 
				 && l[1] > -1 
				 && l[2] > -1 );
	};

	char saferead(Terrain::Terrain * terrain, int l[3]){
		if(inbounds(terrain, l)) 
			return Terrain::read(l);
		return 0;
	}

	void mesher(Terrain::Terrain * terrain, int origin[3], int chunkroot, MeshBuffer*mesh){

		int vertexIndex = 0;
		int indexIndex = 0;
		int vertexCount = 0;

		for (int z = 0; z <= chunkroot; ++z)
		for (int y = 0; y <= chunkroot; ++y)
		for (int x = 0; x <= chunkroot; ++x)
		// The order algorithmically does not matter, but cache cares, and likes zyx, zxy and yzx
		 {	

		 	// AC = chunk location
		 	// AW = world location

			// int AC[] = {x, y, z};
			int AW[] = {
				origin[0]+x,
				origin[1]+y,
				origin[2]+z
			};

			int a = 0;

			// check if block ALPHA is inside the region. ALPHA cannot be in negative, so no check
			if( AW[0] < terrain->root[0] 
			 && AW[1] < terrain->root[1]
			 && AW[2] < terrain->root[2] 
			) a = Terrain::read(AW);



			for (int i = 0; i < 3; ++i) { // face checks


				int BW[] = {
					origin[0]+x,
					origin[1]+y,
					origin[2]+z
				};

				BW[i]--;

				int b = 0; // BLOCK B

				// another aabb-style check
				if( BW[0] < terrain->root[0] 
				 && BW[1] < terrain->root[1]
				 && BW[2] < terrain->root[2] 
				 && BW[0] > -1 
				 && BW[1] > -1 
				 && BW[2] > -1 
				) b = Terrain::read(BW);

				if ((a>0)==(b>0)) continue;

				int side = i;

				// int BC = {x, y, z};
				// BC[i]--;

				float rgb[3];

				int flag = 0; // IF WE ARE A

				int AIR[] = {
					BW[0],
					BW[1],
					BW[2],
				};
				AIR[i]++;

				if (b==0) {
					AIR[i]--;
					flag++;
					side+=3;
					BW[i]++;

					rgb8bit(rgb, a);
				} else{
					rgb8bit(rgb, b);
				}

				// BW IS THE BLOCK WE ARE MESHING
				// 



				int computedAO[4];
				int inverse = 0;

				for (int j = 0; j < 4; ++j){

					int l[3];

					for (int s = 0; s < 3; ++s) l[s] = AIR[s] + aolut[flag][i][j][0][s];
					int side1  = (saferead( terrain, l ) > 0);

					for (int s = 0; s < 3; ++s) l[s] = AIR[s] + aolut[flag][i][j][1][s];
					int corner = (saferead( terrain, l ) > 0);

					for (int s = 0; s < 3; ++s) l[s] = AIR[s] + aolut[flag][i][j][2][s];
					int side2  = (saferead( terrain, l ) > 0);

					int ao = vertexAO( side1>0, side2>0, corner>0 );

					computedAO[j] = ao;
					
					// ao = 3;

					for (int k = 0; k < 3; ++k){
						mesh->vertexBuffer[vertexIndex] = vertices[side][j * 3 + k] + BW[k];

						mesh->colorBuffer[vertexIndex++] = rgb[k] * (0.8 + pow( (float)ao/3.0, 2)  *0.2) * colors[side][j*3+k];
						// mesh->colorBuffer[vertexIndex++] = rgb[k] * (0.5 + ( ao==3 )  *0.5);						
						// colors[side][j*3+k]*
						
					}
				}

				if( computedAO[1] + computedAO[3] < computedAO[0] + computedAO[2] )
					inverse = 1;

				for (int i = 0; i < 6; ++i) mesh->indexBuffer[indexIndex++]   = indices[inverse][i] + vertexCount;
				vertexCount+=4;
			}
		}
		mesh->vertexCount = vertexIndex;
		mesh->indexCount = indexIndex;
	}



	static void* threadMain(void*arg){
		using namespace TerrainMesher;
		int i = 0;

		int bsize = 8*1024*1024;
		mesh.vertexBuffer = (float*)		malloc(bsize*sizeof(float));
		mesh.colorBuffer  = (float*)		malloc(bsize*sizeof(float));
		mesh.indexBuffer  = (unsigned int*)	malloc(bsize*sizeof(int));

		while(threadState != COMMAND_KILL) {

			if( i == chunks.count) {
				printf("All chunks meshed!\n");
				break;
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

			mesher( Terrain::getTerrain(), l, chunks.root, &mesh);

			threadState = SLEEP;
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
		printf("TerrainMesher::init::Allocating %i...\n", chunks.count);
		chunks.x =				(int*)			malloc(sizeof(int) * chunks.count );
		chunks.y =				(int*)			malloc(sizeof(int) * chunks.count );
		chunks.z =				(int*)			malloc(sizeof(int) * chunks.count );
		chunks.vertexBuffer =	(unsigned int*)	malloc(sizeof(int) * chunks.count );
		chunks.indexBuffer =	(unsigned int*)	malloc(sizeof(int) * chunks.count );
		chunks.colorBuffer =	(unsigned int*)	malloc(sizeof(int) * chunks.count );
		chunks.items =			(unsigned int*)	malloc(sizeof(int) * chunks.count );
		chunks.state =			(int*)			malloc(sizeof(int) * chunks.count );



		printf("TerrainMesher::init::Writing...\n");
		int i = 0;
		for (int x = 0; x < terrainroot[0]/chunkroot; ++x)
		for (int y = 0; y < terrainroot[1]/chunkroot; ++y)
		for (int z = 0; z < terrainroot[2]/chunkroot; ++z)
		{
			chunks.x[i]=x;
			chunks.y[i]=y;
			chunks.z[i]=z;

			chunks.state[i] = 0;
			chunks.items[i] = 0;

			i++;
		}


		printf("TerrainMesher::init::Starting thread...\n");
		pthread_create( &thread, NULL, &threadMain, NULL);
	}



	// ChunkProtoMesh create(Terrain::Terrain* terrain) {

	// 	ChunkProtoMesh mesh;

	// 	int bsize = 64*1024*1024;

	// 	int root = terrain->root;

	// 	mesh.vertexBuffer = (float*)		malloc(bsize*sizeof(float));
	// 	mesh.indexBuffer  = (unsigned int*)	malloc(bsize*sizeof(unsigned int));

	// 	clock_t start = clock();

	// 	int vertexIndex = 0;
	// 	int indexIndex = 0;
	// 	int vertexCount = 0;

		

	// 	clock_t end = clock();

	// 	float ms = (float)(end - start) / CLOCKS_PER_SEC * 1000;

	// 	printf("Meshed in %fms\n", ms);


	// 	mesh.vertexCount = vertexIndex;
	// 	mesh.indexCount = indexIndex;

	// 	return mesh;
	// }

	// // loc is in block coordinates



}
