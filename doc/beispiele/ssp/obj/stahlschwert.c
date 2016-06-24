#include <properties.h>
#include <combat.h>
inherit "std/weapon";
 
create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Ein Stahlschwert");
  SetProp(P_LONG, "Dieses Schwert hat eine Klinge aus reinem Stahl, mit der man seine Gegner\nwohl ziemlich gut niedermachen kann. Das Schwert ist recht schwer und\nlaesst sich nur mit zwei Haenden fuehren.\n");
  SetProp(P_NAME, "Stahlschwert");
  SetProp(P_GENDER, 0);
  SetProp(P_WC, 175);
  SetProp(P_VALUE, 1750 + random(100));
  SetProp(P_WEIGHT, 2926);
  SetProp(P_WEAPON_TYPE, WT_SWORD);
  SetProp(P_DAM_TYPE, DT_SLASH);
  SetProp(P_NR_HANDS, 2);
  SetProp(P_MATERIAL, MAT_STEEL);
  AddId(({"schwert", "stahlschwert", "\nsspstahlschwert"}));
}
