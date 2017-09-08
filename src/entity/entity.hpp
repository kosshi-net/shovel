#ifndef SHOVEL_ENTITY_INCLUDED
#define SHOVEL_ENTITY_INCLUDED 1


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
		unsigned long GUID;
		float location[3];
		float velocity[3];
		float speed;
		float acceleration;
		float drag;
		float yaw;
		float pitch;
		float roll;
		float rotation[3];
	} Entity;

	Entity* getEntityByLocation( int );

	int init();

	int entityCount();

	void addComponent	( Entity* e, Component c );
	void removeComponent( Entity* e, Component c );
	bool hasComponent	( Entity* e, Component c);
	void setComponents	( Entity* e, int c );
	bool hasComponents  ( Entity* e, int c);
}

#endif