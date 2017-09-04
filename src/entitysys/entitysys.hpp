namespace ES 
{

	typedef enum {
		COMPONENT_NONE = 0,
		COMPONENT_LOCATION = 1 << 0,
		COMPONENT_VELOCITY = 1 << 1
	} Component;

	typedef struct {
		int mask;
		float location[3];
		float velocity[3];
	} Entity;

	Entity * Entities;

	int init();

	void addComponent	( Entity* e, Component c );
	void removeComponent( Entity* e, Component c );
	bool hasComponent	( Entity* e, Component c);
	void setComponents	( Entity* e, int c );
}