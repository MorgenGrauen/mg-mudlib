#pragma strong_types, save_types, rtt_checks, pedantic, no_clone
inherit "/std/room";

#include <config.h>
#include <properties.h>

protected void create() {
  ::create();

  SetProp(P_INT_LONG, "Ein leerer Raum.\n");
  SetProp(P_INT_SHORT, "Im leeren Raum");
  SetProp(P_LIGHT, 1);

  AddItem(__PATH__(0)"monster", REFRESH_REMOVE);
  AddItem(__PATH__(0)"keks", REFRESH_REMOVE);
  AddItem(__PATH__(0)"goldklumpen", REFRESH_REMOVE);
  AddItem(__PATH__(0)"zuhoerer", REFRESH_REMOVE);

  AddExit("gilde", "/gilden/abenteurer");
}
