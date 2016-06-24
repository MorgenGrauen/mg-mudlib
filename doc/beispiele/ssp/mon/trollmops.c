#include <properties.h>
#include "../files.h"

inherit "std/npc";

create()
{
  if (!clonep(this_object())) return;
   ::create();
   SetProp(P_SHORT, "Ein Trollmops");
   SetProp(P_LONG, 
   "Dies ist einer der sagenumwobenen, blutruenstigen Trollmoepse. Hervor-\n"+
   "gegangen aus der misslungenen Kreuzung zwischen einem Troll und einem\n"+
   "Mops, terrorisieren diese grausamen Bestien jetzt die Welt. Flieh, so-\n"+
   "lange Du noch kannst.\n");
   SetProp(P_NAME, "Trollmops");
   SetProp(P_GENDER, 1);
   AddId("trollmops");
   AddId("troll");
   AddId("mops");
   SetProp(P_RACE, "Troll");
   SetProp(P_LEVEL, 20);
   SetProp(P_MAX_HP, 350);
   SetProp(P_ALIGN, -735 - random(100));
   SetProp(P_HANDS, ({" mit blutverschmierten Krallen", 200 +random(10)}));
   SetProp(P_XP, 350000);
   SetProp(P_BODY, 140 + random(40));
   SetProp(P_SIZE, random(20)+105);  /* Da sind die Groessen der Eltern drin */
   set_living_name("trollmops");
   SetProp(P_AGGRESSIVE, 1);
}
