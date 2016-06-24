#include <properties.h>

inherit "/std/thing";

create() {
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Ein altes Blech");
  SetProp(P_LONG, "Das sind die Ueberreste eines Metallmonsters.\n");
  SetProp(P_NAME, "Blech");
  SetProp(P_GENDER, 0);
  AddId("blech");
  AddAdjective(({"alt", "altes"}));
  SetProp(P_WEIGHT, 1100);
  SetProp(P_VALUE, random(QueryProp(P_MAX_HP))+100);
}
