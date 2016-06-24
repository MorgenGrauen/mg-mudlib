inherit "std/room";

#include <properties.h>
#include <rooms.h>
#include <moving.h>
#include "../files.h"

create()
{
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_SHORT, "Metallebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Der voellig mit Metall\n"+
  "verkleidete Gang macht hier eine Biegung von Osten nach Norden, wo er\n"+
  "in die schwarze Dunkelheit fuehrt. Deine Schritte hallen hier leicht nach,\n"+
  "es scheint sich wohl ein Hohlraum unter dem Boden zu befinden.\n");
  AddDetail("metall", "Es scheint Eisen oder Stahl zu sein.\n");
  AddDetail(({"eisen", "stahl"}), "Du weisst nicht genau, ob es Eisen oder Stahl ist.\n");
  AddDetail(({"gang", "roehre"}), "Der Gang macht einen Knick.\n");
  AddDetail("dunkelheit", "Sie verschlingt alles.\n");
  AddDetail(({"wand", "waende", "boden", "decke"}), "Alles ist aus Metall.\n");
  AddDetail("hohlraum", "Du kannst keine Hinweise auf einen Zugang finden.\n");
  AddDetail("zugang", "Du siehst keinen.\n");
  AddDetail(({"knick", "biegung"}), "So nennt sich sowas halt ... ich kann nix dafuer!\n");
  AddItem(MON("kampfblech"), REFRESH_REMOVE);
  
  AddExit("norden", L1("m1x4"));
  AddExit("osten",L1("m2x5"));
}
