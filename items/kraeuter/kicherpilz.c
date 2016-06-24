// (c) 2001 by Padreic (Padreic@mg.mud.de)
// Beschrieben von Magdalena :o) 08.08.03

#pragma strong_types,rtt_checks

#include <properties.h>
#include <items/kraeuter/kraeuter.h>
#include <items/kraeuter/kraeuterliste.h>

inherit STDPLANT;

#define BS(x) break_string(x, 78)

void create()
{
  ::create();
  customizeMe(KICHERPILZ);
  SetProp(P_NAME,     "Kicherpilz");
  SetProp(P_NAME_ADJ, "lustig");
  SetProp(P_GENDER,   MALE);
  SetProp(P_LONG,     
    "Er hat ein gruenes Kaeppchen auf und ist fuer einen Pilz relativ gross. Ab und\n"
   +"an zuckt er und kichert albern. Daher hat er wohl auch seinen Namen.\n");
  SetProp(PLANT_ROOMDETAIL, 
    "Ein besonders praechtiges Exemplar eines Kicherpilzes winkt dir\n"
   +"aufdringlich zu, also ob es scharf darauf waere, gepflueckt zu werden.\n");
  SetProp(P_SHORT,    "Ein Kicherpilz");
  AddId(({ "pilz", "kicherpilz" }));
  
  AddDetail("kaeppchen",BS(
     "Der Pilz scheint sich fuer die neueste Mode zu interessieren. "
    +"Mit dem gruenen Kaeppchen ist er nach dem letzten Schrei gekleidet."));
  AddDetail("mode",BS(
     "Du schaust an deiner Kleidung hinunter - naja, mit dem Pilz "
    +"kannst du nicht mithalten."));
  AddDetail("schrei",BS(
     "Sei ehrlich: Bei dem Gruen kann man nur schreien!"));
  AddDetail("gruen",BS(
     "Eigentlich ist es doch ganz gut, sich nicht fuer Mode zu "
    +"interessieren, denn das Gruen schmerzt in den Augen."));
 
  set_next_reset(200+random(200));
}

void reset()
{
   set_next_reset(200+random(200));
   if (environment()) 
      tell_object(environment(), "Der Kicherpilz kichert Dich an.\n"); 
   ::reset();
}
