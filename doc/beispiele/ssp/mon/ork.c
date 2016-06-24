#include <properties.h>
#include "../files.h"

inherit "std/npc";

create()
{
  if (!clonep(this_object())) return;
   ::create();
   SetProp(P_SHORT, "Ein grimmiger Ork");
   SetProp(P_LONG, "Der Ork schaut Dich unfreundlich an und wedelt mit seinem Krummschwert.\n");
   SetProp(P_NAME, "Ork");
   SetProp(P_GENDER, 1);
   AddId("ork");
   SetProp(P_RACE, "Ork");
   SetProp(P_LEVEL, 10);
   SetProp(P_MAX_HP, 130);
   SetProp(P_ALIGN, -250);
   SetProp(P_XP, 90000);
   SetProp(P_BODY, 50);
   SetProp(P_SIZE, random(20)+110);
   set_living_name("ork");
   seteuid(getuid(this_object()));
   AddItem(OBJ("krummschwert"),CLONE_WIELD);
   AddItem(OBJ("orkhose"),CLONE_WEAR);
}
