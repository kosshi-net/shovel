#include <cmath>
#include <entity/entity.hpp>
#include <input/input.hpp>
#include <input/keys.hpp>

#include <system/commands.hpp>

using namespace ECS;

namespace LocalInputSystem {

	void addCommand( Entity * e, Command c ){
		e->cmd = e->cmd | c;
	}

	bool hasCommand ( Entity* e, Command c) {
		return ( c & e->cmd ) == c;
	}
	bool hadCommand ( Entity* e, Command c) {
		return ( c & e->last_cmd ) == c;
	}

	void clearCommands( Entity * e ){
		e->cmd = 0x00;
	}

	void tick(){
		glfwPollEvents();

		int count = entityCount();
		for (int i = 0; i < count; ++i) {
			Entity*e = getEntityByLocation(i);

			if( !hasComponents( e, 
				COMPONENT_INPUT_LOCAL )
			) continue;

			e->last_cmd = e->cmd;

			clearCommands( e );

			if (input::getKey(KEY_W)) 
				addCommand( e, CMD_MOVE_FORWARD );

			if (input::getKey(KEY_S)) 
				addCommand( e, CMD_MOVE_BACKWARD );

			if (input::getKey(KEY_A)) 
				addCommand( e, CMD_MOVE_LEFT );

			if (input::getKey(KEY_D)) 
				addCommand( e, CMD_MOVE_RIGHT );




			if (input::getMouseButton(MOUSE_BUTTON_LEFT)) 
				addCommand( e, CMD_ATTACK );

			if (input::getMouseButton(MOUSE_BUTTON_RIGHT)) 
				addCommand( e, CMD_ATTACK2 );

		}

	}
	void mouse(){
		glfwPollEvents();
		double rx, ry;
		input::getCursorMovement(&rx, &ry);

		int count = entityCount();
		for (int i = 0; i < count; ++i) {
			Entity*e = getEntityByLocation(i);

			if( !hasComponents( e, 
				COMPONENT_INPUT_LOCAL )
			) continue;

			e->yaw+=rx*0.1f;
			e->pitch-=ry*0.1f;
		}

	}


}