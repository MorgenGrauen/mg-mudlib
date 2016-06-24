// MorgenGrauen MUDlib
//
// player/viewcmd.h -- view command handling
//
// $Id: viewcmd.h 6408 2007-08-07 23:02:18Z Zesstra $

#ifndef __PLAYER_VIEWCMD_H__
#define __PLAYER_VIEWCMD_H__

// properties

#define P_BLIND              "blind"
#define P_BRIEF              "brief"

#endif // __PLAYER_VIEWCMD_H__

#ifdef NEED_PROTOTYPES

#ifndef __PLAYER_VIEWCMD_H_PROTO__     
#define __PLAYER_VIEWCMD_H_PROTO__

// prototypes
varargs string env_descr(int allow_short,int flags, int force_short );

#endif // __PLAYER_VIEWCMD_H_PROTO__

#endif // NEED_PROTOYPES
