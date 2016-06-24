// (c) by Padreic (Padreic@mg.mud.de)

inherit "std/armour";

#include "../files.h"
#include <combat.h>

void create()
{
   ::create();
   SetProp(P_NAME, "Blaetterkleid");
   SetProp(P_SHORT, "Ein Blaetterkleid");
   SetProp(P_GENDER, NEUTER);
   SetProp(P_LONG,
     "Das Blaetterkleid stammt von einer Waldfee und war genau auf ihre Rundungen\n"
    +"und ihre Groesse abgepasst. Du hast leider nicht die geringste Chance, dieses\n"
    +"Kleid jemals tragen zu koennen.\n");
   AddId(({"kleid", "blaetterkleid"}));
   SetProp(P_MATERIAL, MAT_MISC_WOOD);
   SetProp(P_ARMOUR_TYPE, AT_ARMOUR);
   SetProp(P_VALUE, 500);
   SetProp(P_NOBUY, 1);
   SetProp(P_AC, 10);
   SetProp(P_WEAR_FUNC, ME);
}

int WearFunc(object me, int silent)
{
   if (!PL || query_once_interactive(PL) || getuid(PL)!=getuid() ||
       PL->QueryProp(P_RACE)!="Fee") {
      if (!silent)
        write("Dieses Kleid war genau auf die Rundungen und die Groesse der Waldfee\n"
             +"angepasst von der es stammt. Du hast leider nicht die geringste Chance,\n"
             +"dieses Kleid jemals tragen zu koennen.\n");
      return 0;
   }
   return 1;
}
