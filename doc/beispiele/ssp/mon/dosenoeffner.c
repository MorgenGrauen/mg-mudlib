#include <properties.h>
#include "../files.h"

inherit MMONST;

create()
{
  if (!clonep(this_object())) return;
   ::create();
   AddId(({"dosenoeffner", "oeffner"}));
   AddAdjective(({"mobiler", "mobilen"}));
   SetProp(P_SHORT, "Ein mobiler Dosenoeffner");
   SetProp(P_LONG, "Der Dosenoeffner moechte Dir sicherlich den Kopf mit seinen scharfen\nSchneideraedern oeffnen.\n");
   SetProp(P_NAME, "Dosenoeffner");
   SetProp(P_GENDER, 1);
   SetProp(P_LEVEL, 12);
   SetProp(P_MAX_HP, 120);
   SetProp(P_ALIGN, -123);
   SetProp(P_BODY, 80);
   SetProp(P_HANDS, ({" mit scharfen Schneideraedern", 50}));
   SetProp(P_XP, 30000);
   SetProp(P_SIZE, random(20)+40);
   set_living_name("dosenoeffner");
}
