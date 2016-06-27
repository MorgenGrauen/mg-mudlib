#ifndef _MNPC_H_
#define _MNPC_H_

/*
     folgende Propertys koennen in mnpc's gesetzt werden...
*/

// bitte diese #define inheriten und nicht direkten Pfad benutzen
#define MNPC                     "/p/service/padreic/mnpc/mnpc.c"
#define MNPC_MOVING              "/p/service/padreic/mnpc/moving.c"

// bitte dieses #define benutzen und nicht den direkten Pfad verwenden
#define WALK_MASTER              "/p/service/padreic/mnpc/walk_master"
#define MAX_MASTER_TIME          180

// ist automatisch in jedem mnpc gesetzt
#define P_MNPC                    "mnpc"

// MNPC_DELAY + random(MNPC_RANDOM) ergibt den Abstand zwischen dem laufen
#define MNPC_DELAY                "mnpc_delay"
#define MNPC_RANDOM               "mnpc_random"

// diese NPC wird aufgerufen nachdem der NPC einen Raum betreten hat.
#define MNPC_FUNC                 "mnpc_func"

// Raum angeben in dem der NPC zu Hause ist (dorthin wird im reset ggf. gemovt)
// falls nicht angegeben, ist es der Ort des Clonens
#define MNPC_HOME                 "mnpc_home"

// array die die area in der der NPC sich bewegen darf abgrentzt
#define MNPC_AREA                 "mnpc_area"

// wie lange laeuft der NPC bis er stehen bleibt
#define MNPC_WALK_TIME            "mnpc_walk_time"

// hier koennen einige Flags gesetzt werden
#define MNPC_FLAGS                "mnpc_flags"

// die einzelnen Flags...
#define MNPC_WALK                   0x01  /* NPC ist ein Laufnpc */
#define MNPC_FOLLOW_PLAYER          0x02  /* NPC verfolgt Spieler */
#define MNPC_FOLLOW_ENEMY           0x06  /* NPC verfolgt feindliche Spieler */
#define MNPC_ONLY_EXITS             0x08  /* NPC nutzt keine SpecialExits */
#define MNPC_NO_WALK_IN_FIGHT       0x10  /* NPC bleibt im Kampf stehen */
#define MNPC_GO_HOME_WHEN_STOPPED   0x20  /* NPC wartet nicht bis zum reset */
// wird intern in non livings gesetzt - manuelles setzen nicht noetig
// non livings koennen jedoch _nie_ special exits nutzen...
#define MNPC_DIRECT_MOVE            0x40  /* NPC laeuft mit move statt mit command_me */
// Manche Magier wollen in MNPC_AREA keine Substrings, sondern exakte
// Filenamen angeben. Wenn dieses Flag gesetzt wird, werden die Strings in
// MNPC_AREA exakt geprueft, nicht mit strstr()
#define MNPC_EXACT_AREA_MATCH      0x80
// Manche Magier wollen nicht, dass der MNPC sich im reset nach Hause bewegt
#define MNPC_NO_MOVE_HOME          0x100

/*
   Folgendes sind interne Defaultwerte falls die werte nicht gesetzt werden.
*/

// default delay wird genommen, falls kein bestimmtes eigenes angegeben wird
#define MNPC_DFLT_DELAY           30
// standardhome falls kein besseres home ermittelbar ist
#define MNPC_DFLT_HOME            "/room/void"
// der NPC bleibt nach dieser Zeit stehen und geht im reset nach Hause
#define MNPC_DFLT_WALK            360              /* vorerst 6 min. */
// Diese Funktion wird aufgerufen wenn der NPC einen raum betritt

/*
   interne defines
*/
#define MNPC_PURSUER             "mnpc:pursuer"  /* nur intern */
#define MNPC_LAST_MEET           "mnpc_last_meet"

#endif // _MNPC_H_
