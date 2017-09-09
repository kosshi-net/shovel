#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <log/log.hpp>

/******************************
 *	ENTITY COMPONENT SYSTEM   *
 ******************************/



#define MAX_ENTITY_COUNT 128

namespace ECS 
{

	typedef enum {
		COMPONENT_NONE					= 0,

		COMPONENT_LOCATION				= 1 << 0,
		COMPONENT_VELOCITY				= 1 << 1,
		COMPONENT_AIRDRAG				= 1 << 2,

		COMPONENT_CAMERA				= 1 << 3,
		COMPONENT_LOCALCONTROL			= 1 << 4,

		// COMPONENT_GRAVITY
		// COMPONENT_VOXELGRID				= 1 << 2,
		// COMPONENT_VOXELMESH				= 1 << 3,
		// COMPONENT_PHYSICS				= 1 << 6,
		// COMPONENT_COLLIDER				= 1 << 7,
		// COMPONENT_CULLER					= 1 << 8,
		// COMPONENT_GUID					= 1 << 9,

		COMPONENT_PLACEHOLDER 			= 1 << 31,
	} Component;


	typedef struct {
		int mask; // Componentmask
		int controlmask;
		unsigned long GUID;
		float location[3];
		float velocity[3];
		float drag;
		float acceleration;
		float speed;
		float yaw, pitch, roll;
		float rotation[3];
	} Entity;

	Entity * Entities;
	unsigned long GUID_counter = 0;

	int init(){
		Logger::log("ENTITYSYS :: INIT");
		Entities = (Entity*) calloc( MAX_ENTITY_COUNT, sizeof(Entity) );
		assert( Entities != NULL );
		Logger::log("ENTITYSYS :: FINISHED");

		return 1;
	}	

	int entityCount(){
		return MAX_ENTITY_COUNT;
	}

	Entity* getEntityByLocation(int i){
		return &Entities[i];
	}

	void addComponent    ( Entity* e, Component c ) {
		e->mask = e->mask | c;
	}

	void removeComponent( Entity* e, Component c ) {
		e->mask = e->mask & ~ c;
	}

	void setComponents  ( Entity* e, int c ) {
		e->mask = c;
	}

	bool hasComponent ( Entity* e, Component c) {
		return ( c & e->mask ) == c;
	}
	bool hasComponents ( Entity* e, int c) {
		return ( c & e->mask ) == c;
	}

	unsigned long newGUID(){
		return ++GUID_counter;
	}


}