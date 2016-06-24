#include <properties.h>
#include <combat.h>
inherit "std/armour";
 
create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Ein leichtes Kettenhemd");
  SetProp(P_LONG, "Ein nicht sonderlich gut gearbeitetes Kettenhemd. Anziehen kann man\nes aber trotzdem.\n");
  SetProp(P_NAME, "Kettenhemd");
  SetProp(P_GENDER, 0);
  SetProp(P_ARMOUR_TYPE, AT_ARMOUR);
  SetProp(P_AC, 18);
  SetProp(P_WEIGHT, 1700);
  SetProp(P_VALUE, 1200 +random(100));
  SetProp(P_MATERIAL, MAT_MISC_METAL);
  AddId(({"kettenhemd", "hemd"}));
}
