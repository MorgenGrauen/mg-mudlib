// MorgenGrauen MUDlib
//
// living/put_and_get.h -- living put and get header
//
// $Id: put_and_get.h 6930 2008-08-09 09:20:16Z Zesstra $
 
#ifndef __LIVING_PUT_AND_GET_H__
#define __LIVING_PUT_AND_GET_H__

// properties
#define P_DROP_MSG        "drop_message" 
#define P_PUT_MSG         "put_message"
#define P_PICK_MSG        "pick_message"
#define P_GIVE_MSG        "give_message"
#define P_SHOW_MSG        "show_message"

// If you don't want put_and_get send a message, set the property to
#define NO_PNG_MSG       -1

// For user defined drop messages look for man P_DROP_MSG

#endif // __LIVING_PUT_AND_GET_H__

// prototypes
#ifdef NEED_PROTOTYPES

#ifndef __LIVING_PUT_AND_GET_H_PROTO__
#define __LIVING_PUT_AND_GET_H_PROTO__

object *find_obs(string str, int meth);

void add_put_and_get_commands();

#endif // __LIVING_PUT_AND_GET_H_PROTO__

#endif	// NEED_PROTOYPES

