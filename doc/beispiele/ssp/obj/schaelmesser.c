inherit "std/weapon";

#include <combat.h>
#include <properties.h>

create()
{
  if (!clonep(this_object())) return;
   ::create();
   SetProp(P_SHORT, "Ein Knochenschaelmesser");
   SetProp(P_LONG, 
	   "Ein Messer so lang, wie tausend grosse Zaehne\n"+
	   "Wie tausend grosse Zaehne, ein Messer so lang\n"+
	   "So lang, wie tausend grosse, grosse Zaehne\n");
   SetProp(P_NAME, "Knochenschaelmesser");
   SetProp(P_GENDER, 0);
   AddId(({"knochenschaelmesser", "schaelmesser", "messer"}));
   SetProp(P_WC, 145);
   SetProp(P_NR_HANDS, 1),
   SetProp(P_WEAPON_TYPE, WT_KNIFE);
   SetProp(P_DAM_TYPE, DT_SLASH);
   SetProp(P_VALUE, 1610);
   SetProp(P_WEIGHT, 1150);
   SetProp(P_MATERIAL, MAT_MISC_METAL);
}
