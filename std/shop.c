#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/room";
inherit "/std/room/shop";

#include <properties.h>

protected void create()
{
  if (object_name(this_object()) == __FILE__[0..<3])
  {
    set_next_reset(-1);
    return;
  }
  room::create();
  shop::create();

  SetProp(P_LIGHT, 1);
  SetProp(P_INDOORS,1);
  SetProp(P_INT_SHORT, "StdLaden");
  SetProp(P_INT_LONG,
   "Der Magier, der diesen Laden erschuf, fand keine Beschreibung dafuer.\n");
  AddItem("/obj/entsorg",REFRESH_REMOVE);
}

protected void create_super() {
  set_next_reset(-1);
}

void reset()
{
  room::reset();
  shop::reset();
}
