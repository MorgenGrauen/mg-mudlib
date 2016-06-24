#include <properties.h>
#include "../files.h"

inherit "std/npc";

create()
{
  if (!clonep(this_object())) return;
   ::create();
   SetProp(P_SHORT, "Ein Troll");
   SetProp(P_LONG, "Ein Troll wie aus dem Bilderbuch: Gross, haesslich und dumm.\n");
   SetProp(P_NAME, "Troll");
   SetProp(P_GENDER, 1);
   SetProp(P_RACE, "Troll");
   AddId("troll");
   SetProp(P_LEVEL, 15);
   SetProp(P_MAX_HP, 200);
   SetProp(P_ALIGN, -417);
   SetProp(P_XP, 120000);
   SetProp(P_BODY, 35);
   SetProp(P_SIZE, random(40)+199);
   set_living_name("troll");
   seteuid(getuid(this_object()));
   AddItem(OBJ("trolldolch"),CLONE_WIELD);
   AddItem(OBJ("steinring"),CLONE_WEAR);
   AddItem(OBJ("lkhemd"),CLONE_WEAR);
}
      
