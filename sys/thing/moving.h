// MorgenGrauen MUDlib
//
// thing/moving.h -- object moving
//
// $Id: moving.h 8892 2014-08-04 19:48:28Z Zesstra $
 
#ifndef __THING_MOVING_H__
#define __THING_MOVING_H__

// properties
#define P_NEVERDROP           "neverdrop"
#define P_NODROP              "nodrop"    // can't be dropped
#define P_NOGET               "noget"     // can't be taken

#define P_NOINSERT_MSG        "noinsert_msg"      // Fehlermeldung bei ME_CANT_BE_INSERTED
#define P_NOLEAVE_MSG         "noleave_msg"       // Fehlermeldung bei ME_CANT_LEAVE_ENV
#define P_ENV_TOO_HEAVY_MSG   "env_too_heavy_msg" // Fehlermeldung bei ME_TOO_HEAVY_FOR_ENV
#define P_TOO_MANY_MSG        "too_many_msg"      // Fehlermeldung bei ME_TOO_MANY_OBJECTS
#define P_TOO_HEAVY_MSG       "too_heavy_msg"     // Fehlermeldung bei ME_TOO_HEAVY

#define P_LAST_MOVE           "last_move"
#define P_LAST_CONTENT_CHANGE "last_content_change"

#define P_SENSITIVE     "sensitive" // should be registered as sensitive object


#endif // __THING_MOVING_H__

#ifdef NEED_PROTOTYPES

#ifndef __THING_MOVING_H_PROTO__
#define __THING_MOVING_H_PROTO__

// prototypes
// move the object from current position to dest using the method
public varargs int move(object|string dest, int method);

// remove the object return 1 for successful destruction
public varargs int remove(int silent);

#endif // __THING_MOVING_H_PROTO__

#endif //  // NEED_PROTOYPES
