inherit "std/armour";
 
#include <properties.h>                    
#include <combat.h>
 
create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Ein Orkhelm");
  SetProp(P_LONG, "Ein stabiler Helm, der schon ausgiebig von einem stinkenden Ork\ngetragen wurde.\n");
  SetProp(P_NAME, "Orkhelm");
  SetProp(P_GENDER, 1);
  SetProp(P_ARMOUR_TYPE, AT_HELMET);
  SetProp(P_AC, 7);
  SetProp(P_WEIGHT, 450);
  SetProp(P_VALUE, 350);
  SetProp(P_MATERIAL, ([MAT_MISC_METAL:95, MAT_SLIME:5]));
  AddId(({"helm", "orkhelm"}));
}
