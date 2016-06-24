inherit "std/npc";

#include <properties.h>
#include "../files.h"

create()
{
  if (!clonep(this_object())) return;
   ::create();
   SetProp(P_SHORT, "Ein Anfuehrer der Orks");
   SetProp(P_LONG, "Dies ist ein fetter, stinkender Ork-Haeuptling, der Dir alles \nandere als freundlich gesonnen ist.\n");
   SetProp(P_NAME, "Ork");
   SetProp(P_GENDER, 1);
   AddId(({"ork", "anfuehrer", "haeuptling"}));
   SetProp(P_RACE, "Ork");
   SetProp(P_LEVEL, 16);
   SetProp(P_MAX_HP, 220);
   SetProp(P_ALIGN, -280);
   SetProp(P_XP, 160000);
   SetProp(P_BODY, 60);
   SetProp(P_SIZE, random(20)+130);
   set_living_name("orkchef");
   seteuid(getuid(this_object()));
   AddItem(OBJ("saebel"),CLONE_WIELD);
   AddItem(OBJ("orkhose"),CLONE_WEAR);
   AddItem(OBJ("orkhelm"),CLONE_WEAR);
}

      
