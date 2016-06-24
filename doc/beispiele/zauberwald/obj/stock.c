// (c) by Padreic (Padreic@mg.mud.de)

inherit "std/weapon";

#include "../files.h"
#include <combat.h>

void create()
{
   ::create();
   SetProp(P_SHORT, "Ein kleiner Zeigestock");
   SetProp(P_NAME, "Zeigestock");
   SetProp(P_GENDER, MALE);
   SetProp(P_LONG,
     "Ein kleiner Stock, mit dem man gut Dinge zeigen oder in den Sand malen kann.\n");
   AddAdjective("klein");
   AddId(({"stock", "zeigestock"}));
   SetProp(P_WEAPON_TYPE, WT_STAFF);
   SetProp(P_SIZE, 120);
   SetProp(P_DAM_TYPE, DT_BLUDGEON);
   SetProp(P_NR_HANDS, 1);
   SetProp(P_WC, 120);
   SetProp(P_VALUE, 300);
   SetProp(P_NOBUY, 1);
   SetProp(P_MATERIAL, MAT_MISC_WOOD);
}
