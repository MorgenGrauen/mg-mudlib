#include <properties.h>
#include <combat.h>
inherit "std/armour";
 
create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Ein Steinring");
  SetProp(P_LONG, "Ein kleiner Ring, voellig aus einem grauen Stein.\n");
  SetProp(P_NAME, "Steinring");
  SetProp(P_GENDER, 1);
  SetProp(P_ARMOUR_TYPE, AT_RING);
  SetProp(P_AC, 2);
  SetProp(P_WEIGHT, 270);
  SetProp(P_VALUE, 300 +random(150));
  SetProp(P_MATERIAL, MAT_MISC_STONE);
  AddId(({"ring", "steinring"}));
}
