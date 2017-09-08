#include <cstdlib>
#include <log/log.hpp>

namespace Memory {

	unsigned int total = 0;

	void* w_malloc( unsigned int size ) {
		Logger::log( "MEMORY :: Allocating %i bytes..." );
		void* ptr = malloc( size );

		if( ptr == NULL ) {
			Logger::error( "MEMORY :: Malloc failure!" );
			exit( EXIT_FAILURE );
		}

		return ptr;
	}

	void* w_calloc( unsigned int size ) {
		Logger::log( "MEMORY :: Allocating %i bytes..." );
		void* ptr = calloc( size );

		if( ptr == NULL ) {
			Logger::error( "MEMORY :: Calloc failure!" );
			exit( EXIT_FAILURE );
		}

		return ptr;
	}

	void w_free( void*ptr ){
		free(ptr);
	}





}
/*

	void * createBlock( unsigned int size ) {
		return w_malloc( size );
	};

	createPartitionTable( void* block, unsigned int size, unsigned int maxPartitions){

		int*b = (int*)block;

		p[0] = size;
		p[1] = maxPartitions;
		for (int i = 0; i < maxPartitions; ++i)
		{
			p[2 + i * 2] = 0;
			p[3 + i * 2] = 0;
		}
	};
	/*
		Writes a table to the block
		[ uint, memsize ] [ uint, partcount ]
		[ uint, pointer ] [ uint, size  ]
		[ uint, pointer ] [ uint, size  ]
		...

		first integer is the size of the whole block
		second integer is the count of partitions
		after this the table begins

		each element of the table has 2 values, pointer and a size

		if pointer is NULL (0), theres no partition
		pointers are an offset from the first value of the whole block


		nevermind that, requirements are:

		it should fast to find all the available blocks

	* /

	int partCount(void* block){
		return *((int*)block[1]);
	}

	int partTableSize(){
		return sizeof(int) * ( 2 + partCount() * 2 );
	}

	alloc( void* block, unsigned int size ) {

		// loop all blocks
			// 
		
		

	}

}


// MEMORY MANAGER, 6.9.2017 

