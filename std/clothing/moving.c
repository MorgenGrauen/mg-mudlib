// MorgenGrauen MUDlib
//
// armour/moving.c -- armour moving object
//
// $Id: moving.c,v 3.3 1998/03/02 08:34:57 Paracelsus Exp $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

inherit "/std/thing/moving";

#define NEED_PROTOTYPES 1

#include <thing/properties.h>
#include <properties.h>
#include <moving.h>
#include <defines.h>
#include <clothing.h>

/* Bekleidung muss vor Bewegung und Zerstoerung ausgezogen werden */

varargs int move(mixed dest, int method ) {
  // ggf. Ausziehen
  if (objectp(QueryProp(P_WORN)))
    DoUnwear(method & (M_SILENT|M_NOCHECK));

  if ((method&M_NOCHECK) || (!(object)QueryProp(P_WORN)))
    return ::move(dest, method);

  return ME_CANT_BE_DROPPED;
}

varargs int remove(int silent) {
  // ggf. Ausziehen
  if (objectp(QueryProp(P_WORN)))
    DoUnwear(M_SILENT|M_NOCHECK);

  if (!(object)QueryProp(P_WORN))
    return ::remove(silent);
  // Ausziehen hat irgendwie nicht geklappt. :-(
  return 0;
}

