#include <properties.h>
#include <combat.h>
inherit "std/weapon";
 
create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Ein Krummschwert");
  SetProp(P_LONG, "Das Krummschwert sieht aus, als haette es einmal einem Ork gehoert.\n");
  SetProp(P_NAME, "Krummschwert");
  SetProp(P_GENDER, 0);
  SetProp(P_WC, 130 + random(15));
  SetProp(P_VALUE, 1200 + random(200));
  SetProp(P_WEIGHT, 1100 + random(100));
  SetProp(P_WEAPON_TYPE, WT_SWORD);
  SetProp(P_DAM_TYPE, DT_SLASH);
  SetProp(P_NR_HANDS, 1);
  SetProp(P_MATERIAL, MAT_MISC_METAL);
  AddId("schwert");
  AddId("krummschwert");
}
