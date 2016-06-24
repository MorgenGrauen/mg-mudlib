#include <properties.h>
#include <combat.h>
#include "../files.h"

inherit MMONST;

create()
{
  if (!clonep(this_object())) return;
   ::create();
   SetProp(P_SHORT, "Der Knochenschaeler");
   SetProp(P_LONG,
   "Der Knochenschaeler geht um, geht um\n"+
   "Der Knochenschaeler geht um, geht um\n"+
   "Sein Messer so lang, wie tausend grosse Zaehne\n"+
   "Wie tausend grosse Zaehne, sein Messer so lang\n"+
   "Er schneidet Dich in Stuecke, portionsgerecht und fein\n"+
   "Er schneidet Dich in Stuecke, und keiner tuts ihm gleich\n"+
   "Er schaelt die Knochen, er quaelt Deinen Geist\n"+
   "Er schaelt die Knochen, er quaelt Deinen Geist\n"+
   "Habet acht, habet acht! Seiet vorsichtig, habet Acht!\n"+
   "Schau Dich um, schau Dich um\n"+
   "Schau Dich um, Du wirst der naechste sein\n");
   SetProp(P_NAME, "Knochenschaeler");
   SetProp(P_GENDER, 1);
   AddId(({"schaeler", "knochenschaeler"}));
   SetProp(P_LEVEL, 18);
   SetProp(P_MAX_HP, 300);
   SetProp(P_ALIGN, -789);
   SetProp(P_HANDS, ({" mit seinem langen Messer", 145}) );
   SetProp(P_BODY, 110);
   SetProp(P_XP, 220000);
   SetProp(P_AGGRESSIVE, 1);
   SetProp(P_SIZE, 220);
   set_living_name("knochenschaeler");
   clone_object(OBJ("schaelmesser"))->move(this_object());
   command("zuecke messer");
}

QueryArticle(c, d, f)
{
   return ::QueryArticle(c, 1, f);
}
