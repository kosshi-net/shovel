#ifndef SHOVEL_LOCALCONTROL_SYSTEM_INCLUDED 
#define SHOVEL_LOCALCONTROL_SYSTEM_INCLUDED 1

#include <entity/entity.hpp>
#include <system/commands.hpp>

namespace LocalInputSystem {

	void addCommand( ECS::Entity * e, Command c );
	bool hasCommand (  ECS::Entity* e, Command c);
	bool hadCommand (  ECS::Entity* e, Command c);
	void clearCommands(  ECS::Entity * e );

	void tick();
	void mouse();
}

#endif