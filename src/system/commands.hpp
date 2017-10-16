#ifndef SHOVEL_COMMANDS_INCLUDED 
#define SHOVEL_COMMANDS_INCLUDED 1

typedef enum {

	CMD_MOVE_FORWARD	= 1 << 0,
	CMD_MOVE_LEFT		= 1 << 1,
	CMD_MOVE_BACKWARD	= 1 << 2,
	CMD_MOVE_RIGHT		= 1 << 3,
	CMD_ATTACK 			= 1 << 4,
	CMD_ATTACK2			= 1 << 5,

} Command;


#endif