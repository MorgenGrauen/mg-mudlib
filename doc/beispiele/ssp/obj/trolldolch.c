#include <properties.h>
#include <combat.h>
inherit "std/weapon";
 
create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Ein Trolldolch");
  SetProp(P_LONG, "Ein langer, scharfer Dolch, dem Gestank nach muss er einem Troll\ngehoert haben.\n");
  SetProp(P_NAME, "Trolldolch");
  SetProp(P_GENDER, 1);
  SetProp(P_WC, 119);
  SetProp(P_VALUE, 1000 +random(400));
  SetProp(P_WEIGHT, 500 +random(50));
  SetProp(P_WEAPON_TYPE, WT_KNIFE);
  SetProp(P_DAM_TYPE, DT_PIERCE);
  SetProp(P_NR_HANDS, 1);
  SetProp(P_MATERIAL, MAT_MISC_METAL);
  AddId(({"dolch", "trolldolch"}));
}
