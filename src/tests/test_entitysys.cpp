#include <cstdio>
#include <entitysys/entitysys.cpp>

int main()
{
	using namespace ES;

	init();

	addComponent( &Entities[0], COMPONENT_LOCATION );

	printf("TEST, result %i, should be 1\n", hasComponent( &Entities[0], COMPONENT_LOCATION ));
	printf("TEST, result %i, should be 0\n", hasComponent( &Entities[1], COMPONENT_LOCATION ));


	return 0;
}