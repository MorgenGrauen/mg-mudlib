// (c) by Padreic (Padreic@mg.mud.de)

inherit "std/weapon";

#include "../files.h"
#include <combat.h>

void create()
{
   ::create();
   SetProp(P_SHORT, "Ein Zauberstab");
   SetProp(P_NAME, "Zauberstab");
   SetProp(P_GENDER, MALE);
   SetProp(P_LONG,
     "Dies war einmal der Zauberstab einer Waldfee aus dem Zauberwald. In dem Stab\n"
    +"liegt sicherlich noch eine grosse Magie verborgen, doch nutzen kannst Du sie\n"
    +"nicht.\n");
   AddId(({"stab", "zauberstab"}));
   SetProp(P_WEAPON_TYPE, WT_CLUB);
   SetProp(P_DAM_TYPE, ({ DT_MAGIC, DT_BLUDGEON }) );
   SetProp(P_NR_HANDS, 1);
   SetProp(P_WC, 140);
   SetProp(P_VALUE, 800);
   SetProp(P_NOBUY, 1);
   SetProp(P_HIT_FUNC, ME);
   SetProp(P_MATERIAL, ({ MAT_MISC_MAGIC, MAT_MISC_WOOD }) );
}

int HitFunc(object enemy)
{
    object ob;
    ob=QueryProp(P_WIELDED);
    if (ob && !query_once_interactive(ob) && getuid(ob)==getuid() &&
        ob->QueryProp(P_RACE)=="Fee") return 100+random(300);
    return 0;
}
