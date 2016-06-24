// MorgenGrauen MUDlib
//
// player/command.h - Commands & Co
//
// $Id: attributes.h 6282 2007-05-09 20:55:17Z Zesstra $
 
#ifndef __PLAYER_COMMAND_H__
#define __PLAYER_COMMAND_H__

// properties
#define P_DISABLE_COMMANDS "p_lib_disablecommands"

// Indizes fuers Array von commandblock
#define B_OBJECT  0
#define B_TIME    1
#define B_VALUE   2
#define B_EXCEPTIONS 3

#endif // __PLAYER_COMMAND_H__


#ifdef NEED_PROTOTYPES

#ifndef __PLAYER_COMMAND_H_PROTO__
#define __PLAYER_COMMAND_H_PROTO__

// prototypes
mixed modify_command(string str);
int command_me(string cmd);
varargs string _unparsed_args(int level);

// Set- und Query-Methoden
static mixed _set_p_lib_disablecommands(mixed data);
static mixed _query_p_lib_disablecommands();

#endif // __PLAYER_ATTRIBUTES_H_PROTO__

#endif // NEED_PROTOYPES

