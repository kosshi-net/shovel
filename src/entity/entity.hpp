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
		COMPONENT_INPUT_LOCAL			= 1 << 4,
		COMPONENT_CONTROL_MODEL_BASIC	= 1 << 5,
		
		COMPONENT_DEBUG_COLLISION		= 1 << 6,
		COMPONENT_DEBUG					= 1 << 7,

		COMPONENT_AABB_COLLIDER			= 1 << 8,
		COMPONENT_AABB_GEOMETRY			= 1 << 9

		// COMPONENT_GRAVITY
		// COMPONENT_VOXELGRID				= 1 << 2,
		// COMPONENT_VOXELMESH				= 1 << 3,
		// COMPONENT_PHYSICS				= 1 << 6,
		// COMPONENT_COLLIDER				= 1 << 7,
		// COMPONENT_CULLER					= 1 << 8,
		// COMPONENT_GUID					= 1 << 9,

	} Component;


	typedef struct {
		int mask; // Componentmask
		int cmd;
		int last_cmd;
		unsigned long GUID;
		float location[3];
		float last_location[3];
		float velocity[3];
		float min[3];
		float max[3];
		float drag;
		float acceleration;
		float speed;
		float yaw, pitch, roll;
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