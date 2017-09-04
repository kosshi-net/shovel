#include <cstdio>
#include <cstdlib>
#include <log/log.hpp>

#define MAX_ENTITY_COUNT 128;

namespace ES 
{

	typedef enum {
		COMPONENT_NONE = 0,
		COMPONENT_LOCATION = 1 << 0,
		COMPONENT_VELOCITY = 1 << 1
	} Component;

	typedef struct {
		int mask; // Stores each component with a single bit
		float location[3];
		float velocity[3];
	} Entity;

	Entity * Entities;

	int init(){
		// Logger::log("ENTITYSYS :: INIT");
		Entities = (Entity*) calloc( sizeof(Entity) * 128, 0 );

		return 1;
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
		return ( e->mask & c ) != 0;
	}
}