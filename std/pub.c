// MorgenGrauen MUDlib
//
// pub.c -- Alles, was eine Kneipe braucht.
//
// $Id: pub.c 6571 2007-10-21 14:41:10Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/room";
inherit "/std/room/pub";

#include <properties.h>

protected void create()
{
  room::create();
  pub::create();
  SetProp(P_LIGHT,1);
  SetProp(P_INDOORS,1);
  SetProp(P_INT_LONG,
   "Der Magier, der diesen Pub erschuf, war leider zu faul, eine "+
   "Beschreibung\ndafuer herzustellen.\n");
  SetProp(P_INT_SHORT,"generic pub");
  AddItem("/obj/topliste",REFRESH_REMOVE);
}

protected void create_super() {
      set_next_reset(-1);
}

void reset()
{
   room::reset();
   pub::reset();
}





































