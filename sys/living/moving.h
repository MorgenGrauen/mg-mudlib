// MorgenGrauen MUDlib
//
// living/moving.h -- Props/Prototypen fuer living/moving.c
//
// $Id: moving.h 8892 2014-08-04 19:48:28Z Zesstra $

#ifndef __LIVING_MOVING_H__
#define __LIVING_MOVING_H__

#define P_PURSUERS       "pursuers"

#endif // __LIVING_MOVING_H__

#ifdef NEED_PROTOTYPES

#ifndef __LIVING_MOVING_H_PROTO__
#define __LIVING_MOVING_H_PROTO__

public void AddPursuer(object ob);
public void RemovePursuer(object ob);

public void TakeFollowers();

#ifdef __THING_MOVING_H_PROTO__
// das darf nicht passieren, weil move in thing anders ist als hier. *seufz*
inconsistent_move_protoyp_error
#endif
public varargs int move( object|string dest, int methods, string direction,
                         string textout, string textin );
public varargs int remove(int silent);

// internal
public void _SetPursued(object ob);
public void _RemovePursued(object ob);


#endif // __LIVING_MOVING_H_PROTO__

#endif // NEED_PROTOTYPES

