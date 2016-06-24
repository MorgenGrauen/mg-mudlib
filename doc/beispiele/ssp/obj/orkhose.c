#include <properties.h>
#include <combat.h>
inherit "std/armour";
 
create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Eine Orkhose");
  SetProp(P_LONG, "Die Beinbekleidung von Orks.\n");
  SetProp(P_NAME, "Hose");
  SetProp(P_GENDER, 2);
  SetProp(P_ARMOUR_TYPE, AT_TROUSERS);
  SetProp(P_AC, 7);
  SetProp(P_WEIGHT, 555);
  SetProp(P_VALUE, 480 + random(40));
  SetProp(P_MATERIAL, ([MAT_CLOTH:99, MAT_SHIT:1]));
  AddId(({"hose", "hosen", "orkhose", "orkhosen"}));
}
