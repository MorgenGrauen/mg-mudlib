// MorgenGrauen MUDlib
//
// clothing.h -- clothing header
//
// $Id: clothing.h 6065 2006-09-29 23:22:52Z Zesstra $
 
#ifndef __CLOTHING_H__
#define __CLOTHING_H__

// properties
#define P_WORN          "worn"         // Objekt mit dem Traeger der Ruestung

#define P_WEAR_FUNC     "wear_func"    // Objekt das eine WearFunc definiert
#define P_REMOVE_FUNC   "remove_func"  // Objekt das eine RemoveFunc definiert

#define P_WEAR_MSG      "wear_msg"     // Meldungen beim Anziehen
#define P_UNWEAR_MSG    "unwear_msg"   // Meldungen beim Ausziehen

#define P_LAST_WEAR_ACTION "last_wear_action" // Letzte Aktion des Spielers
                                              // An- und ausziehen betreff

// Die Wear-Actions im einzelnen

#define WA_WEAR            0
#define WA_UNWEAR          1

#endif // __CLOTHING_H__

#ifdef NEED_PROTOTYPES

#ifndef __CLOTHING_H_PROTO__
#define __CLOTHING_H_PROTO__

// prototypes
// functions that will be called if one issues a wear command (or to manually
// wear this armour
varargs int do_wear(string str, int silent);

// function that will be called if one issues an unwear command or to manually
// unwear the armour
varargs int do_unwear(string str, int silent);
// function is the rest of do_unwear() -- msgs etc
varargs int DoUnwear(int silent, int all);

int ziehe(string str);

#endif // __CLOTHING_H_PROTO__

#endif	// NEED_PROTOYPES
