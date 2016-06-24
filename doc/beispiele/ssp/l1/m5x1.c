inherit "std/room";

#include <properties.h>
#include "../files.h"

create()
{
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_SHORT, "Steinebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Der grob behauene Gang\n"+
  "macht hier eine Biegung von Sueden nach Westen, wo er in einem schwarzen\n"+
  "Loch verschwindet, aus dem ein fuerchterlicher Gestank dringt.\n"); 
  AddDetail("gang", "Der Gang fuehrt in die Dunkelheit.\n");
  AddDetail("dunkelheit", "Schwarze, lichtlose Dunkelheit.\n");
  AddDetail("loch", "Hier lauert irgend etwas Schreckliches.\n");
  AddDetail("gestank", "Der Gestank ist wirklich grauenhaft.\n");
  AddDetail("boden", "Hoppla, hier liegen Fleischfetzen rum, im Westen scheint wohl was zu wueten.\n");
  AddDetail(({"fleischfetzen", "fetzen", "fleisch"}), "Ist ja eklig!\n");
  AddDetail(({"wand", "waende"}), "Die Waende sind verschmiert.\n");
  AddDetail("decke", "Gaehn.\n");
  AddExit("westen",L1("m4x1"));
  AddExit("sueden",L1("m5x2"));
}
