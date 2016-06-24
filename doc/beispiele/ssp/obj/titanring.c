inherit "std/armour";

#include <properties.h>
#include <combat.h>

create()
{
  ::create();
  SetProp(P_SHORT, "Ein Titanring");
  SetProp(P_LONG, "Ein Ring aus Titan.\n");
  SetProp(P_NAME, "Titanring");
  SetProp(P_GENDER, 1);
  AddId(({"titanring", "ring"}));
  SetProp(P_ARMOUR_TYPE, AT_RING);
  SetProp(P_AC, 2);
  SetProp(P_WEIGHT, 280);
  SetProp(P_VALUE, 520);
}
