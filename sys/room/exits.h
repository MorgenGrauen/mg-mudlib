// MorgenGrauen MUDlib
//
// room/exits.h -- exit specific defines
//
// $Id: exits.h 9477 2016-02-19 21:17:51Z Zesstra $

#ifndef __ROOM_EXITS_H__
#define __ROOM_EXITS_H__

// Properties

#define P_EXITS              "exits"
#define P_SPECIAL_EXITS      "special_exits"
#define P_HIDE_EXITS         "hide_exits"

#define P_BLOCK_MSG          "block_msg"

#endif // __ROOM_EXITS_H__

// Prototypes
#ifdef NEED_PROTOTYPES

#ifndef __ROOM_EXITS_H_PROTO__
#define __ROOM_EXITS_H_PROTO__

static mapping _set_exits(mapping map_ldfied) ;
static mapping _query_exits();
static int _set_special_exits(mapping map_ldfied);
static mapping _query_special_exits();
static string _MakePath(string str);
void AddExit(string|string* cmd, closure|string dest);
protected void _AddExit(string|string* cmd, string|closure room,
                        string message);
void RemoveExit(string|string* cmd);
void AddSpecialExit(string|string* cmd, string|closure functionname);
void RemoveSpecialExit(string|string* cmd);
varargs string GetExits( object viewer );
int _normalfunction();

#endif // __ROOM_EXITS_H_PROTO__

#endif // NEED_PROTOTYPES
