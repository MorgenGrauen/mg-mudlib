// MorgenGrauen MUDlib
//
// walking.h -- Definitionen und Properties fuer laufende INPCs
//
// $Id: walking.h,v 3.1 1997/02/12 13:17:14 Wargon Exp $

#ifndef __INPC_WALKING_H__
#define __INPC_WALKING_H__

// Properties and types
#define P_INPC_LAST_PLAYER_CONTACT "inpc_last_player_contact"
#define P_INPC_LAST_ENVIRONMENT "inpc_last_environment"
#define P_INPC_WALK_MODE "inpc_walk_mode"
#define P_INPC_WALK_DELAYS "inpc_walk_delay"
#define P_INPC_WALK_FLAGS "inpc_walk_flags"
#define P_INPC_WALK_AREA "inpc_walk_area"
#define P_INPC_WALK_ROUTE "inpc_walk_route"
#define P_INPC_HOME "inpc_home"

#define WF_MAY_LOAD 0x01
#define WF_MAY_FOLLOW 0x02
#define WF_MAY_USE_SPECIAL 0x04
#define WF_MAY_WALK_BACK 0x08

#define WM_STOP 0
#define WM_RANDOM 1
#define WM_ROUTE 2
#define WM_WALKTO 3
#define WM_FOLLOW 4
#define WM_FLEE 5

#endif // __INPC_WALKING_H__

#ifdef NEED_PROTOTYPES
#ifndef __INPC_WALKING_H_PROTOTYPES__
#define __INPC_WALKING_H_PROTOTYPES__

// Prototypen
int may_enter_room(mixed room);

#endif // __INPC_WALKING_H_PROTOTYPES__
#endif // NEED_PROTOTYPES

