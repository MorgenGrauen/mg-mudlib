// MorgenGrauen MUDlib
//
// player/quests.h -- player quest handling
//
// $Id: quest.h,v 3.2 2000/02/09 15:56:25 Padreic Exp $
 
#ifndef __PLAYER_QUESTS_H__
#define __PLAYER_QUESTS_H__

// properties

#define P_QUESTS             "quests"
#define P_QP                 "questpoints"

#endif // __PLAYER_QUESTS_H__

#ifdef NEED_PROTOTYPES

#ifndef __PLAYER_QUESTS_H_PROTO__
#define __PLAYER_QUESTS_H_PROTO__ 

// prototypes

varargs int GiveQuest(string questname, string message);
int QueryQuest(string questname);

#endif // __PLAYER_QUESTS_H_PROTO__

#endif // NEED_PROTOYPES
