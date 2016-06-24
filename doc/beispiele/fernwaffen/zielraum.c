inherit "/std/room";

#include "./path.h"
#include <properties.h>

void create() {
  ::create();

  SetProp(P_LIGHT, 1);
  SetProp(P_INT_SHORT, "Auf einer Lichtung");
  SetProp(P_INT_LONG, break_string(
    "Auf dieser Lichtung steht ein Baum, der verdaechtig viele Aeste hat, "
    "von denen man auf die Lichtung gut sehen kann."));
  SetProp(P_NEVER_CLEAN, 1); // damit der Raum auch da bleibt

  AddItem(__PATH__(0)"npc", REFRESH_REMOVE);

  AddExit("schussraum", __PATH__(0)"schussraum");
}
