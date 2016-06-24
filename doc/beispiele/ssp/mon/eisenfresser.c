#include <properties.h>
#include "../files.h"

inherit MMONST;

create()
{
  if (!clonep(this_object())) return;
   ::create();
   AddId(({"eisenfresser", "fresser"}));
   SetProp(P_SHORT, "Ein Eisenfresser");
   SetProp(P_LONG,
   "Eisenfresser ernaehren sich ausschliesslich von Eisen, allerdings sind\n"+
   "sie so kurzsichtig, dass sie erstmal in alles reinbeissen, was sie\n"+
   "irgendwie wahrnehmen.\n");
   SetProp(P_NAME, "Eisenfresser");
   SetProp(P_GENDER, 1);
   SetProp(P_LEVEL, 14);
   SetProp(P_MAX_HP, 150);
   SetProp(P_ALIGN, -215);
   SetProp(P_BODY, 75);
   SetProp(P_HANDS, ({" mit eisenbeschlagenen Zaehnen", 55}));
   SetProp(P_XP, 42000);
   SetProp(P_AGGRESSIVE, 1);
   SetProp(P_SIZE, random(200)+50);   /* Naja, kurzsichtig halt... ;-) */
   set_living_name("eisenfresser");
   AddItem( OBJ("eisenklumpen") );
}
