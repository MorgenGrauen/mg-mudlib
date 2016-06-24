// (c) by Padreic (Padreic@mg.mud.de)

inherit "std/weapon";

#include "../files.h"
#include <combat.h>

void create()
{
   ::create();
   SetProp(P_SHORT, "Ein langer Kamm");
   SetProp(P_NAME, "Kamm");
   SetProp(P_GENDER, MALE);
   SetProp(P_LONG,
     "Dieser schoene Kamm gehoerte einmal einer Waldfee die damit immer ihr schoenes\n"
    +"langes Haar kaemmte...\n");
   AddAdjective("kamm");
   AddId("kamm");
   SetProp(P_WEAPON_TYPE, WT_CLUB);
   SetProp(P_DAM_TYPE, DT_PIERCE);
   SetProp(P_NR_HANDS, 1);
   SetProp(P_WC, 100);
   SetProp(P_VALUE, 100);
   SetProp(P_NOBUY, 1);
   SetProp(P_MATERIAL, ({ MAT_MISC_MAGIC, MAT_HORN }) );
   AddCmd("kaemme", "cmd_kaemmen");
}

static int cmd_kaemmen(string str)
{
   notify_fail("Was moechtest Du denn kaemmen?\n");
   if (str!="haare") return 0;
   write("Eitel wie Du bist, versuchst Du Dir mit dem Kamm die Haare zu kaemmen, doch\n"
        +"irgendwie ist er ein bisschen zu gross fuer Deine Haare.\n");
   say(BS("Eitel wie "+PL->QueryPronoun()+" ist, kaemmt sich "+PL->name(WER)
         +" die Haare. Doch irgendwie ist der Kamm ein bisschen zu gross fuer "
         +PL->QueryPossPronoun(NEUTER, WEN, 1)+" kurzen Haare."));
   return 1;
}
