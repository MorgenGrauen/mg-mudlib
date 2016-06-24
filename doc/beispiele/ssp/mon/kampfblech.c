#include <properties.h>
#include <combat.h>
#include "../files.h"

inherit "std/npc";

create()
{
  if (!clonep(this_object())) return;
   ::create();
   SetProp(P_SHORT, "Ein Kampfblech");
   SetProp(P_LONG, 
   "Das Kampfblech sieht irgendwie aus wie ein normales Backblech, nur\n"+
   "viel gefaehrlicher!\n");
   SetProp(P_NAME, "Kampfblech");
   SetProp(P_GENDER, 0);
   AddId(({"blech", "kampfblech","\nkampfblech"}));
   SetProp(P_LEVEL, 8);
   SetProp(P_MAX_HP, 100);
   SetProp(P_ALIGN, -33);
   SetProp(P_BODY, 70);
   SetProp(P_HANDS, ({" mit eisenharten Schlaegen", 60}));
   SetProp(P_XP, 30000);
   SetProp(P_SIZE, 40);
   SetProp(P_RACE, "Metallmonster");
   SetProp(P_VULNERABILITY, ({ DT_WATER }));
   SetProp(P_NOCORPSE, 1);
   SetProp(P_DIE_MSG, " faellt laut scheppernd um.\n");
   set_living_name("kampfblech");
}

die()
{
   clone_object(OBJ("kampfblech"))->move(environment());
   ::die();
}
