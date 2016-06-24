// MorgenGrauen MUDlib
//
// moving.h -- Methoden und Rueckgabewerte fuer/von move()
//             Dieser Header sollte IMMER benutzt werden, wenn ein Objekt
//             move() benutzt!
//
// $Id: moving.h 8801 2014-05-04 21:30:59Z Zesstra $

#ifndef _MOVING_
#define _MOVING_

// leider nicht via <> moeglich, da dieses File benutzt wird, bevor der Master
// den Include-Dir-Hook gesetzt hat.
#include "/sys/living/moving.h"
#include "/sys/player/moving.h"

/*
 * moving.h
 *
 * How objects are moving around and reasons why they cant do it.
 */

/* Methods to move objects */
#define M_GO      1  /* we walk around */
#define M_TPORT 128  /* We teleport to another place */
#define M_NOCHECK 2  /* just plain move it without any checks */
#define M_GET     4  /* obj geht von nonliving zu living */
#define M_PUT     8  /* obj geht von living zu nonliving */
#define M_GIVE   16  /* obj geht von living zu living */
#define M_SILENT 32  /* obj soll ohne meldung ausgezogen werden */
#define M_NO_ATTACK 512 /* Kein Begruessungsschlag */
#define M_NO_SHOW 256 /* Zeige nicht die Room-Descr beim ankommen */

/* Hier folgt noch was fuer die Unit */
#define M_MOVE_ALL 32

/* Errors when moving things */
#define MOVE_OK                   1 // gar kein Fehler. ;-)
/* Hey, I'm a player and cant be simply carried away */
#define ME_PLAYER 	         -1
/* Cant move into container because I'm too heavy */
#define ME_TOO_HEAVY             -2
#define ME_CANT_TPORT_IN         -3 /* Cant teleport there ! */
#define ME_CANT_TPORT_OUT        -4 /* Cant teleport from here ! */
#define ME_CANT_BE_DROPPED       -5 /* You cant drop the feel... this thing */
#define ME_CANT_BE_TAKEN         -6 /* Nope, you cannot take me */
#define ME_CANT_BE_INSERTED      -7 /* Container prevents me from inserting */
#define ME_CANT_LEAVE_ENV        -8 /* Container prevents me from leaving */
/* Can't move into container, its Env would get too heavy */
#define ME_TOO_HEAVY_FOR_ENV     -9
#define TOO_MANY_OBJECTS         -10 /* too many objects in new environment */
#define ME_NOT_ALLOWED           -11 /* Sorry - off limits for players */
// Object was destructed during move/in init()
#define ME_WAS_DESTRUCTED        -12
// object itself does not want to be moved for undefined reason
#define ME_DONT_WANT_TO_BE_MOVED -13

// check for valid move error - mainly for internal use!
#define ALL_MOVE_ERRORS ({ME_PLAYER,ME_TOO_HEAVY,ME_CANT_TPORT_IN,\
                          ME_CANT_TPORT_OUT,ME_CANT_BE_DROPPED,\
                          ME_CANT_BE_TAKEN,ME_CANT_BE_INSERTED,\
                          ME_CANT_LEAVE_ENV,ME_TOO_HEAVY_FOR_ENV,\
                          TOO_MANY_OBJECTS,ME_NOT_ALLOWED,\
                          ME_WAS_DESTRUCTED,ME_DONT_WANT_TO_BE_MOVED})
#define VALID_MOVE_ERROR(x) (member(ALL_MOVE_ERRORS,x) != -1)

#define NO_TPORT 1
#define NO_TPORT_IN 2
#define NO_TPORT_OUT 4

/* Ablehnreaktionen von NPCs */
#define REJECT_LIGHT_MODIFIER "reject_light_modifier"
#define REJECT_DROP           "reject_drop"
#define REJECT_GIVE           "reject_give"
#define REJECT_KEEP           "reject_keep"

/* Zugriffsmethoden fuer find_obs */

#define PUT_GET_NONE 0x00
#define PUT_GET_TAKE 0x01
#define PUT_GET_DROP 0x02

#endif
