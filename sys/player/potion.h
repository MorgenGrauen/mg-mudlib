// MorgenGrauen MUDlib
//
// player/potion.h -- potion services
//
// $Id: potion.h 9117 2015-01-25 11:25:14Z Zesstra $

#ifndef __PLAYER_POTION_H__
#define __PLAYER_POTION_H__

// properties

#define P_POTIONROOMS         "potionrooms"
#define P_KNOWN_POTIONROOMS   "known_potionrooms"
#define P_TRANK_FINDEN        "trank_finden"

// veraltete Props, lang unbenutzt.
#define P_VISITED_POTIONROOMS "visited_potionrooms"
#define P_BONUS_POTIONS       "bonus_potions"

#endif // __PLAYER_POTION_H__

#ifdef NEED_PROTOTYPES

#ifndef __PLAYER_POTION_H_PROTO__
#define __PLAYER_POTION_H_PROTO__ 

// prototypes

varargs int FindPotion(string s);

#endif // __PLAYER_POTION_H_PROTO__

#endif // NEED_PROTOYPES

