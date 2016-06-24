inherit "std/npc";

#include <properties.h>
#include <combat.h>
#include "../files.h"

create()
{
  if (!clonep(this_object())) return;
   ::create();
   SetProp(P_NOCORPSE, 1);
   SetProp(P_RACE, "Metallmonster");
   SetProp(P_DIE_MSG, " zerfaellt in seine Bestandteile.\n");
   SetProp(P_VULNERABILITY, ({ DT_WATER }));
}

die()
{
   create_blech();
   ::die();
}

create_blech()
{
   object b;
   b = clone_object(OBJ("blech"));
   b->move(environment(this_object()));
}
