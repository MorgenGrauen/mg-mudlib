// MorgenGrauen MUDlib
//
// rooms.h -- standard room header
//
// $Id: rooms.h 8425 2013-02-23 18:49:09Z Zesstra $

#ifndef __ROOM_H__
#define __ROOM_H__

#include <room/description.h>
#include <room/exits.h>
#include <doorroom.h>

// properties
// Flags fuer Einschraenkung der Daten fuer Mappingzwecke an den Client
#define P_MAP_RESTRICTIONS  "lib_p_map_restrictions"

// raise error message
#define AddDoor(a,b,c,d,e,f) raise_error("Use NewDoor instead of AddDoor!\n")
#define AddDoor2(a,b,c,d,e,f,g) raise_error("Use NewDoor instead of AddDoor2!\n")
#define AddRoomCmd(a,b) raise_error("Use AddCmd instead of AddRoomCmd!\n")
#define RemoveRoomCmd(a,b) raise_error("Use RemoveCmd instead of RemoveRoomCmd!\n")
// defines

/* entries in item array */
#define RITEM_OBJECT  0
#define RITEM_FILE    1
#define RITEM_REFRESH 2
#define RITEM_PROPS   3

/* refresh for extra items */
#define REFRESH_NONE      0
#define REFRESH_DESTRUCT  1
#define REFRESH_REMOVE    2
#define REFRESH_ALWAYS    3
#define REFRESH_MOVE_HOME 4

/* values for teleport forbid property */
#define TPORT_IN   1
#define TPORT_OUT  2
#define TPORT_BOTH (TPORT_IN | TPORT_OUT)

/* values for different roomtypes */
#define RT_SHOP    1
#define RT_PUB     2

// Werte fuer P_MAP_RESTRICTIONS
#define MR_NOUID  0x1
#define MR_NOINFO 0x2

#endif // __ROOM_H__

#ifdef NEED_PROTOTYPES

#ifndef __ROOM_H_PROTO__
#define __ROOM_H_PROTO__

// prototypes
// no prototypes

#endif // __ROOM_H_PROTO__

#endif	// NEED_PROTOYPES
