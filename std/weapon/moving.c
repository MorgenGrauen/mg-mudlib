// MorgenGrauen MUDlib
//
// weapon/moving.c -- Bewegen/Zerstoeren von Waffen
//
// $Id: moving.c 6312 2007-05-20 22:40:51Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

inherit "/std/thing/moving";

//#define NEED_PROTOTYPES 1

#include <thing/properties.h>
#include <properties.h>
#include <moving.h>
#include <defines.h>

varargs int DoUnwield(int silent);

varargs int move(mixed dest, int method)
{
  DoUnwield( method & (M_SILENT|M_NOCHECK));
  if ((method & M_NOCHECK) || (!QueryProp(P_WIELDED)))
    return ::move(dest, method );
}

varargs int remove(int silent )
{
  DoUnwield( silent );
  if (!QueryProp(P_WIELDED))
    return ::remove( silent );
}
