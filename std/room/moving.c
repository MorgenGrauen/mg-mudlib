// MorgenGrauen MUDlib
//
// room/moving.c -- Entfernen von Raeumen
//
// $Id: moving.c 8041 2012-03-19 18:38:21Z Zesstra $

#pragma strong_types
#pragma save_types
#pragma pedantic
#pragma range_check
#pragma no_clone

public varargs int remove(int silent)
{
  destruct( this_object() );
  return 1;
}
