// MorgenGrauen MUDlib
//
// npc/put_and_get.c.c -- Geben und nehmen fuer NPCs
//
// $Id: put_and_get.c 6371 2007-07-17 22:46:50Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "std/living/put_and_get";
#include <moving.h>
#include <properties.h>

void give_notify( object obj )
{
   mixed* msg;
   if (pointerp(msg=QueryProp(P_REJECT))) {
      switch(msg[0]) {
         case REJECT_GIVE:
            say(msg[1]);
            give_obj( obj, this_player() );
            break;
         case REJECT_LIGHT_MODIFIER:
            if (obj->QueryProp(P_LIGHT_MODIFIER) ||
                obj->QueryProp(P_LIGHT)) break;
         case REJECT_DROP:
            say(msg[1]);
            drop_obj( obj );
            break;
         case REJECT_KEEP:
         default: say(msg[1]); /* keep it */
    }
  }
}
