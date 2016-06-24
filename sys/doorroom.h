// MorgenGrauen MUDlib
//
// doorroom.h -- new doors, managed by doormaster
//
// $Id: doorroom.h 9128 2015-01-26 17:42:21Z Arathorn $

#ifndef _DOORROOM_H_
#define _DOORROOM_H_

#define DOOR_MASTER "/obj/doormaster"
#define P_DOOR_INFOS "door_infos"   // Informationen

#define DOOR_OPEN     0x01 // Tuer initial offen
#define DOOR_CLOSED   0x02 // Tuer initial zu
#define DOOR_NEEDKEY  0x08 // Man braucht einen Schluessel zum Oeffnen
#define DOOR_CLOSEKEY 0x10 // Man braucht einen Schluessel zum Schliessen
#define DOOR_RESET_CL 0x20 // Tuer schliesst sich bei jedem reset()
#define DOOR_RESET_OP 0x40 // Tuer oeffnet sich bei jedem reset()

#define D_DEST 0
#define D_CMDS 1
#define D_IDS 2

// properties
// Ab hier die aenderbaren Eigenschaften der Tuer
#define D_FLAGS 3
#define D_LONG 4
#define D_SHORT 5
#define D_NAME 6
#define D_GENDER 7
#define D_FUNC 8
// Funktion, die VOR dem Durchschreiten der Tuer aufgerufen wird (im Startraum)
#define D_MSGS 9
// Messages fuer Move
#define D_FUNC2 10
// Funktion, die NACH dem Durchschreiten der Tuer aufgerufen wird (im Zielraum)
#define D_TESTFUNC 11
// Funktion, die testet, ob die Tuer durchschritten werden darf (im Startraum)
#define D_RESET_MSG 12
// Meldung beim Tuer-Reset (tuer->name(WER,0)+D_RESET_MSG).
#define D_OPEN_WITH_MOVE 13
// Falls gesetzt wird die Tuer auch mit dem Bewegungsbefehl geoeffnet
// und durchschritten, falls oeffnen erfolgreich

#define D_MINPROPS 3
#define D_MAXPROPS 13

#define D_STATUS_LOCKED -2
#define D_STATUS_CLOSED -1
#define D_STATUS_OPEN    1

#endif /* _DOORROOM_H_ */

#ifdef NEED_PROTOTYPES

#ifndef __DOORROOM_H_PROTO__
#define __DOORROOM_H_PROTO__

// prototypes
mapping QueryAllDoors();
varargs int NewDoor(string|string* cmds, string dest, string|string* ids,
                    mapping|<int|string|string*>* props);
void    init_doors ();
string  look_doors ();
void    reset_doors ();
varargs int go_door (string str);
//int     oeffnen (string str);
//int     schliessen (string str);
int     set_doors (string *cmds, int open);
varargs string GetExits(object viewer); 

#endif /* __DOORROOM_H_PROTO__ */
#endif /* NEED_PROTOTYPES */
