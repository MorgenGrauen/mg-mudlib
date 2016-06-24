inherit "std/weapon";

#include <properties.h>                    
#include <combat.h>
 
create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Ein schartiger Saebel");
  SetProp(P_LONG, "Der Saebel ist schartig, koennte aber trotzdem recht brauchbar sein.\n");
  SetProp(P_NAME, "Saebel"); 
  SetProp(P_GENDER, 1);
  SetProp(P_WC, 144);
  SetProp(P_VALUE, 1525);
  SetProp(P_WEIGHT, 1264);
  SetProp(P_WEAPON_TYPE, WT_SWORD);
  SetProp(P_DAM_TYPE, DT_SLASH);
  SetProp(P_NR_HANDS, 1);
  SetProp(P_MATERIAL, MAT_MISC_METAL);
  AddId("saebel");
  AddAdjective(({"schartig", "schartiger", "schartigen"}));
}
