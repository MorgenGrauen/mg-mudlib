#include <properties.h>
#include <combat.h>
#include "../files.h"

inherit MMONST;

create()
{
  if (!clonep(this_object())) return;
   ::create();
   SetProp(P_SHORT, "Ein Stahldrache");
   SetProp(P_LONG,
   "Der Stahldrache ist ein ernstzunehmender Gegner, gepanzert mit Platten\n"+
   "aus reinem Stahl ist es schwer, ihm irgendwie Schaden zuzufuegen.\n");
   SetProp(P_NAME, ({"Stahldrache", "Stahldrachen", "Stahldrachen", "Stahldrachen"}));
   SetProp(P_GENDER, 1);
   AddId(({"drache", "stahldrache", "drachen", "stahldrachen"}));
   SetProp(P_RACE, "Drache");
   SetProp(P_LEVEL, 20);
   SetProp(P_MAX_HP, 400);
   SetProp(P_ALIGN, -250);
   SetProp(P_HANDS, ({" mit staehlernen Klauen", 150}));
   SetProp(P_BODY, 130);
   SetProp(P_XP, 300000);
   SetProp(P_SIZE, random(50)+250);
   SetChats(40, ({
      "Der Stahldrache schnaubt laut.\n",
      "Der Stahldrache schaut Dich mit staehlernem Blick an.\n",
      "Der Stahldrache waelzt sich herum.\n",
      "Der Stahldrache spuckt etwas stahlblaues Feuer.\n",
   }) );
   AddSpell(1, 115, "Der Stahldrache spuckt stahlblaues Feuer auf Dich.\n",
  	            "Der Stahldrache spuckt stahlblaues Feuer auf @WEN.\n",
	    DT_FIRE);
   SetProp(P_SPELLRATE,20);
   clone_object(OBJ("gummistiefel"))->move(this_object());
   clone_object(OBJ("stahlschwert"))->move(this_object());
   set_living_name("stahldrache");
}
